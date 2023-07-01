import urllib.request
import ssl
from bs4 import BeautifulSoup
import json
import math
import re
import pprint
from collections import OrderedDict

# class BTHomeType:
#     def __init__(self, object_id, property, data_type, factor, example, result, unit):
#         self.object_id = object_id
#         self.property = property
#         self.data_type = data_type
#         self.factor = factor
#         self.example = example
#         self.result = result
#         self.unit = unit
# bti = BTHomeType(0x00, "packet id", "uint8 (1 byte)", "1", "9", "9", "")

gcontext = ssl.SSLContext()
page = urllib.request.urlopen(
    "http://bthome.io/format/",
    context=gcontext,
)
page_content = page.read()
TARGET_DIR = "../components/bthome_base/"

soup = BeautifulSoup(page_content, "html.parser")
tables = soup.find_all("table")

# main_types = ["numeric", "binary", None, "numeric"]
main_types = [
    "numeric",  # Object id	Property	Data type	Factor	Example	Result	Unit
    "binary",  # Object id	Property	Data type
    None,  # "event_binary",  # Object id	Device type	Event id	Event type	Event property
    "numeric",  # Object id	Property	Data Type
]  # main types in sections within bthome.io/format page

data = []
for imain in range(4):
    main_type = main_types[imain]
    # skip events for now
    if not main_type:
        continue

    table = tables[imain].find("tbody")
    lastrow = []
    for row in table.findAll("tr"):
        factor = 1
        unit_of_measurement = ""

        row = row.find_all("td")
        row = [ele.text.strip() for ele in row]

        object_id_raw = row[0]
        property = row[1]
        event_is_subevent = None
        if main_type in ["binary", "numeric"]:
            data_type = row[2]
            data_type_signed = data_type.startswith("sint")
            data_type_length = 1
            try:
                data_type_length = int(re.search("\((\d+)", data_type).group(1))
            except:
                pass
        # elif main_type == "event_binary":
        #     if object_id_raw == "":
        #         object_id_raw = row[0] = lastrow[0]
        #         property = row[1] = lastrow[1]
        #         event_is_subevent = True
        #     event_id = row[2]
        #     event_type = row[3] if row[3] != "None" else None
        #     unit_of_measurement = row[4].replace("#", "").replace(" ", "")

        #     example_len = (int)(len(row[5].strip()) / 2)
        #     data_type_length = example_len - 1
        #     data_type = f"uint8 ({data_type_length} byte)"

        #     if event_is_subevent:
        #         object_id_raw = object_id_raw + event_id.replace("0x", "")

        #     property = f"{property}_{event_type}" if event_type else property
        #     data_type_signed = False
        #     data_type_length = 1
        else:
            print("ERROR 1a", main_type)
            continue

        object_id = int(object_id_raw, 16)
        # print(object_id_raw,object_id)

        # sanitize property
        property = (
            property.replace(" ", "_")
            .replace(".", "_")
            .replace("(", "")
            .replace(")", "")
        )

        if main_type == "numeric":
            try:
                factor = float(row[3])
            except:
                pass
        accuracy_decimals = int(abs(math.log10(factor)))

        if main_type == "numeric":
            try:
                unit_of_measurement = row[6]
            except:
                pass

        # generate decode key: DataLen 123 bits | DataType 45 bits | Factor 67 bits
        decode_datatype_key = (
            data_type_length
            | ((1 if data_type_signed else 0) << 3)
            | (accuracy_decimals << 5)
        )

        sensor_data = {
            "property": property,
            "property_unique": property,
            "main_type": main_type,
            "decode_datatype_key": decode_datatype_key,
            "data_type": data_type,
            "data_type_signed": data_type_signed,
            "data_type_length": data_type_length,
            "measurement_type": object_id,
            "measurement_type_hex": object_id_raw,
            "accuracy_decimals": accuracy_decimals,
            "unit_of_measurement": unit_of_measurement,
            # "event_is_subevent": event_is_subevent,
        }

        data.append(sensor_data)

        lastrow = row

# print(list(data[0].values())[0])
data.sort(key=lambda x: x["measurement_type"])

#
# Duplicates: find and rename duplicates
#

differentiator_dictionary = {
    "unit_of_measurement": None,
    # "accuracy_decimals": [
    #     None,
    #     "to_deci",
    #     "to_centi",
    #     "to_milli",
    # ],
    "accuracy_decimals": {-1: "coarse", 0: "None", +1: "precise"},
    "data_type_length": [0, 1, 2, 3, 4],
}
differentiator_candidates = differentiator_dictionary.keys()

for index, item in enumerate(data):
    property_key = item["property"]
    main_type_key = item["main_type"]

    # find all alike item indexes
    alike_list = [
        item
        for (index, item) in enumerate(data)
        if item["property_unique"] == property_key
        and item["main_type"] == main_type_key
    ]
    if len(alike_list) > 1:
        # print(">>>", property_key)
        # find smallest one and mark as default
        min_measurement_type_index, _ = min(
            [(index, item["measurement_type"]) for index, item in enumerate(alike_list)]
        )
        # print(alike_list[min_measurement_type_index])
        alike_list[min_measurement_type_index]["default"] = True

        # rename all based on data_type_length
        # first - find dimenstions in which they differ
        value_matrix = []
        differentiator_matrix = [[] for _ in range(len(differentiator_candidates))]
        for index, item in enumerate(alike_list):
            value_row = [item[candidate] for candidate in differentiator_candidates]
            value_matrix.append(value_row)
            for cindex, candidate in enumerate(differentiator_candidates):
                differentiator_matrix[cindex].append(item[candidate])
        for cindex, crow in enumerate(differentiator_matrix):
            if len(set(crow)) == 1:
                differentiator_matrix[cindex] = []

        base_accuracy_at = None

        def append_diff(item):
            global base_accuracy_at
            # prop_value = item['property']
            meas_value = item["measurement_type"]
            # print(f"  == {item['measurement_type_hex']}")
            retval = ""

            for cindex, crow in enumerate(differentiator_matrix):
                differentiator = list(differentiator_candidates)[cindex]

                # if all items were the same, skip it
                if len(crow) == 0:
                    continue

                # ###
                # if there is only one value with a single occurence and this is the one - use this differentiator for this one, skip others
                value = item[differentiator]

                def group_list(lst):
                    res = [(el, lst.count(el)) for el in lst]
                    return list(OrderedDict(res).items())

                crow1 = group_list(crow)
                crow1.sort(key=lambda x: x[1])
                # print("  ",crow)
                # print(crow1)
                if (
                    crow1[0][1] == 1
                    and len(crow1) > 1
                    and crow1[1][1] > 1
                    and crow1[0][0] != value
                ):
                    # print(f"   skip:1a value:{value}")
                    continue

                # ###
                # accuracy_decimal: default/first elem should not get extension, yet others should receive coarse/precise
                min_value = min([item["measurement_type"] for item in alike_list])
                if min_value == item["measurement_type"]:
                    base_accuracy_at = item
                    # differentiator = list(differentiator_candidates)[cindex]
                    # print(
                    #     f"   skip:1b value:{meas_value} min_value:{min_value} differentiator:{differentiator}"
                    # )
                    continue

                # # not all are different - but this one is unique of the set (e.g. volume_ml) - we use this and skip
                # count_of_this = sum(
                #     map(lambda x: x[differentiator] == item[differentiator], alike_list)
                # )
                # print(
                #     f"  {differentiator} value:{item[differentiator]} count_of_this:{count_of_this} lencrow:{len(crow)} crow:{crow} alike_list:{len(alike_list)}"
                # )
                # if len(crow) <= len(alike_list) and count_of_this == 1:
                #     print("  skip")
                #     continue

                # if count_of_this > 1:
                #     continue

                # get the differentiator value and apply - either from a list or the value itself
                if type(differentiator_dictionary[differentiator]) is list:
                    suffix = differentiator_dictionary[differentiator][
                        int(item[differentiator])
                    ]
                elif (
                    differentiator == "accuracy_decimals"
                ):  # type(differentiator_dictionary[differentiator]) is dict:
                    # accuracy_decimal -- dict, relative to "base_accuracy_value"
                    if base_accuracy_at[differentiator] > item[differentiator]:
                        suffix = "coarse"
                    else:
                        suffix = "precise"
                else:
                    suffix = item[differentiator]
                # print(
                #     f"    {differentiator} value:{str(item[differentiator])} suffix:{suffix}"
                # )
                if suffix:
                    retval += "_" + str(suffix)

                # all are different
                if len(crow) == len(alike_list):
                    break

            # print(f"    retval:{retval}")
            return retval

        for item in alike_list:
            item["property_unique"] += append_diff(item)
            # print(
            #     f"   {item['measurement_type_hex']}: {item['property']} - {item['property_unique']}"
            # )

# for item in data:
#     print(item["property_unique"])

################################################################

fname = TARGET_DIR + "bthome_common_generated.h"
print(f"generating {fname}...")
f = open(fname, "w", encoding="utf-8")
f.write(
    """
/* auto generated, do not edit */

#pragma once
#include <pgmspace.h>

namespace bthome_base
{
"""
)

################################################################


def generate_encoder_enum(data):
    values = []
    for item in data:
        if item["main_type"] == "numeric":
            extension = "VALUE"
        elif item["main_type"] == "binary":
            extension = "STATE"
        # elif item["main_type"] == "event_binary":
        #     extension = "EVENT"
        else:
            extension = "__"
        values.append(
            f'  BTHOME_{item["property_unique"].upper()}_{extension} = {item["measurement_type_hex"]}'
        )
    return "typedef enum {\n" + ", \n".join(values) + "\n} BTHome_e;\n\n"


data1 = generate_encoder_enum(data)
f.write(data1)

################################################################


def generate_decoder_array(data):
    values = []
    maxvalue = max(
        # [item["measurement_type"] for item in data if not item["event_is_subevent"]]
        [item["measurement_type"] for item in data]
    )
    for measurement_type in range(maxvalue):
        item = None
        try:
            item = [
                item2 for item2 in data if item2["measurement_type"] == measurement_type
            ][0]
        except:
            pass
        if item:
            values.append(
                "  "
                + "0b"
                + format((item["decode_datatype_key"]), "08b")
                + f', /* {"0x%02x" %item["measurement_type"]} | {item["property"]} | {item["property_unique"]} | {item["data_type"]} | {item["accuracy_decimals"]} */'
            )
        else:
            values.append("  0b00000000, /* unused */")
    names = "\n".join(values)
    return (
        "static const uint8_t PROGMEM MEAS_TYPES_FLAGS[] = { /* 8th bit Unused | 6-7th bits Factor | 4-5th bits DataType | 1-2-3rd bits DataLen */ \n"
        + names
        + "\n};\n"
    )


data1 = generate_decoder_array(data)
f.write(data1)

################################################################

f.write(
    """
}
"""
)
f.close()

################################################################


def generate_const(data, main_type):
    values = []
    for item in data:
        if item["main_type"] == main_type:
            values.append(
                f'  "{item["property_unique"]}": {{ "measurement_type": {item["measurement_type_hex"]}, "accuracy_decimals": {item["accuracy_decimals"]}, "unit_of_measurement": "{item["unit_of_measurement"]}" }},'
            )
    return "\n".join(values)


fname = TARGET_DIR + "const_generated.py"
print(f"generating {fname}...")
f = open(fname, "w", encoding="utf-8")

for imain in set(main_types):
    if imain is None:
        continue

    f.write(f"MEASUREMENT_TYPES_{imain.upper()}_SENSOR = {{\n")
    data1 = generate_const(data, imain)
    f.write(data1)
    f.write("\n}\n")

f.close()
