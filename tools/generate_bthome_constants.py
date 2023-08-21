import urllib.request
import ssl
from bs4 import BeautifulSoup
import json
import math
import re
import pprint
from collections import OrderedDict

import helpers

# import sys
# sys.path.append("../components")
# import bthome_base.const_generated
# print(bthome_base.const_generated.MEASUREMENT_TYPES_NUMERIC_SENSOR)
# print(sensor.DEVICE_CLASSES)
# print(binary_sensor.DEVICE_CLASSES)

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

ssl._create_default_https_context = ssl._create_unverified_context
page = urllib.request.urlopen("http://bthome.io/format/")
page_content = page.read()
TARGET_DIR = "../components/"
TARGET_BTHOMEBASE_DIR = TARGET_DIR + "/bthome_base/"
TARGET_DOC_DIR = TARGET_DIR + "/docs/"

soup = BeautifulSoup(page_content, "html.parser")
tables = soup.find_all("table")

main_types = [
    "numeric",  # Object id	Property	Data type	Factor	Example	Result	Unit
    "binary",  # Object id	Property	Data type
    "event",  # Object id	Device type	Event id	Event type	Event property
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
        unit_of_measurement = None
        sensor_data = {}

        row = row.find_all("td")
        row = [ele.text.strip() for ele in row]

        object_id_raw = row[0]
        property = property_unique = row[1]
        event_is_subevent = None
        if main_type in ["binary", "numeric"]:
            data_type = row[2]
            data_type_signed = data_type.startswith("sint")
            data_type_length = 1
            try:
                data_type_length = int(
                    re.search("\((\d+)", data_type).group(1))
            except:
                pass
        elif main_type == "event":
            if object_id_raw == "":  # not the first row - object_id is empty
                object_id_raw = row[0] = lastrow[0]
                property = row[1] = lastrow[1]
            device_type = property
            event_id = row[2]
            event_type = row[3].replace("press", "click").lower()
            property_unique = f"{property}_{event_type}"
            unit_of_measurement = row[4].replace("#", "").replace(" ", "")

            example_len = (int)(len(row[5].strip()) / 2)
            data_type_length = example_len - 1
            data_type = f"uint8 ({data_type_length} byte)"

            data_type_signed = False

            sensor_data["event_id"] = int(event_id, 16)
            sensor_data["event_type"] = event_type
            sensor_data["device_type"] = device_type
        else:
            print("ERROR 1a", main_type)
            continue

        object_id = int(object_id_raw, 16)
        object_id_firstbyte = helpers.msb(object_id)
        # print(object_id_raw, object_id)

        # sanitize property
        def sanitize_property(value):
            return (
                value.replace(" ", "_")
                .replace(".", "_")
                .replace("(", "")
                .replace(")", "")
                .lower()
            )
        property = sanitize_property(property)
        property_unique = sanitize_property(property_unique)

        if main_type == "numeric":
            try:
                factor = float(row[3])
            except:
                pass
            try:
                unit_of_measurement = row[6]
            except:
                pass
        accuracy_decimals = int(abs(math.log10(factor)))

        # generate decode key: DataLen 123 bits | DataType 45 bits | Factor 67 bits
        decode_datatype_key = (
            data_type_length
            | ((1 if data_type_signed else 0) << 3)
            | (accuracy_decimals << 5)
        )

        sensor_data.update({
            "property": property,
            "property_unique": property_unique,
            "main_type": main_type,
            "decode_datatype_key": decode_datatype_key,
            "data_type": data_type,
            "data_type_signed": data_type_signed,
            "data_type_length": data_type_length,
            "measurement_type": object_id,
            "measurement_type_hex16": helpers.hex2(object_id_firstbyte, 2),
            "accuracy_decimals": accuracy_decimals,
            "unit_of_measurement": unit_of_measurement,
            "device_class": helpers.find_matching_device_class(
                object_id, property, main_type
            ),
            # "icon": helpers.find_matching_icon(object_id, property, main_type) # NOTE: check and understand how and which default icon is assigned in home assistant
        })

        data.append(sensor_data)

        lastrow = row

# print(list(data[0].values())[0])
data = sorted(data, key=lambda x: helpers.msb(x["measurement_type"]))

#
# Duplicates: find and rename duplicates
#

differentiator_dictionary = {
    "unit_of_measurement": None,
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
            [(index, item["measurement_type"])
             for index, item in enumerate(alike_list)]
        )
        # print(alike_list[min_measurement_type_index])
        alike_list[min_measurement_type_index]["default"] = True

        # rename all based on data_type_length
        # first - find dimenstions in which they differ
        value_matrix = []
        differentiator_matrix = [[]
                                 for _ in range(len(differentiator_candidates))]
        for index, item in enumerate(alike_list):
            value_row = [item[candidate]
                         for candidate in differentiator_candidates]
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
            # print(f"  == {item['measurement_type_hex16']}")
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
                # min_value = min([item["measurement_type"]
                #                 for item in alike_list])
                # if min_value == item["measurement_type"]:
                #     base_accuracy_at = item
                #     # differentiator = list(differentiator_candidates)[cindex]
                #     # print(
                #     #     f"   skip:1b value:{meas_value} min_value:{min_value} differentiator:{differentiator}"
                #     # )
                #     continue

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

        # find first element of alike list
        min_value = min([item["measurement_type"] for item in alike_list])
        for item1 in alike_list:
            if item1["measurement_type"] == min_value:
                base_accuracy_at = item1
                break

        # rename all alike except base
        for item1 in alike_list:
            if base_accuracy_at != item1:
                item1["property_unique"] += append_diff(item1)

# for item in data:
#     print(item["property_unique"])

################################################################


def create_bthome_common_generated(data):

    fname = TARGET_BTHOMEBASE_DIR + "bthome_common_generated.h"
    print(f"generating {fname}...")
    f = open(fname, "w", encoding="utf-8")
    f.write("""
/* auto generated, do not edit */

#pragma once
#include <pgmspace.h>

namespace bthome_base
{
"""
            )

    # ---

    def generate_encoder_enum(data):
        values = []
        event_values = {}
        event_subvalues = {}
        for item in data:
            propname = item["property_unique"]
            value = item["measurement_type_hex16"]
            if item["main_type"] == "numeric":
                extension = "VALUE"
            elif item["main_type"] == "binary":
                extension = "STATE"
            elif item["main_type"] == "event":
                extension = "EVENT"
                value2 = helpers.hex2(item["event_id"], 2)

                device_type = item["device_type"]
                event_type = item["event_type"]

                event_values[device_type] = item["measurement_type"]
                event_first = not device_type in event_subvalues
                if event_first:
                    propname = item["property"]
                    event_subvalues[device_type] = []

                event_subvalues[device_type].append(
                    f'  BTHOME_{device_type.upper()}_{event_type.replace(" ","_").upper()} = {value2}'
                )

                if not event_first:
                    continue
            else:
                extension = "__"
            values.append(
                f'  BTHOME_{propname.upper()}_{extension} = {value}'
            )

        retval = "typedef enum {\n" + ", \n".join(values) + "\n} BTHome_e;\n\n"
        retval += "".join(
            ("typedef enum {\n" +
             ", \n".join(event_subvalues[value])
             + "\n} BTHome_"+value.capitalize()+"_e;\n\n")
            for value in list(event_values)
        )
        return retval

    data1 = generate_encoder_enum(data)
    f.write(data1)

    # ---

    def generate_decoder_array(data):
        values = []
        maxvalue = max(
            # [item["measurement_type"] for item in data if not item["event_is_subevent"]]
            [helpers.msb(item["measurement_type"]) for item in data]
        )
        for measurement_type in range(maxvalue):
            item = None
            try:
                item = [
                    item2 for item2 in data if item2["measurement_type"] == measurement_type
                ][0]
            except:
                pass

            elem = ("  "
                    + f'/* {"0x%02x" % measurement_type} */ '
                    + "0b"
                    + format((item["decode_datatype_key"]
                             if item else 0), "08b")
                    + ', /* '
                    + (f'{item["property_unique"]} | {item["data_type"]} | {item["main_type"]}' +
                       (f' * {math.pow(10,-int(item["accuracy_decimals"]))}' if item["main_type"]
                        == "numeric" else "")
                       if item else 'unused')
                    + ' */'
                    )

            values.append(elem)
        names = "\n".join(values)
        return (
            "static const uint8_t PROGMEM MEAS_TYPES_FLAGS[] = { /* 8th bit Unused | 6-7th bits Factor | 4-5th bits DataType | 1-2-3rd bits DataLen */ \n"
            + names
            + "\n};\n"
        )

    data1 = generate_decoder_array(data)
    f.write(data1)

    # ---

    f.write("""
}
"""
            )
    f.close()


create_bthome_common_generated(data)

################################################################


def create_const_generated(data):

    global main_types

    def generate_const(data, main_type):
        values = {}
        for item in data:
            if item["main_type"] == main_type:
                if (main_type != "event"):
                    convitem = {"measurement_type": item["measurement_type"]}
                    if item["accuracy_decimals"] != None:
                        convitem["accuracy_decimals"] = item["accuracy_decimals"]
                    if item["unit_of_measurement"] != None:
                        convitem["unit_of_measurement"] = item["unit_of_measurement"]
                    if item["device_class"] != None:
                        convitem["device_class"] = item["device_class"]
                    # if item["icon"] != None: # NOTE: check and understand how and which default icon is assigned in home assistant
                    #     convitem["icon"] = item["icon"]
                else:
                    convitem = {
                        "device_event_type": item["measurement_type"] << 8 | item["event_id"],
                        "measurement_type": item["measurement_type"],
                        "event_id": item["event_id"]
                    }
                    if (item["data_type_length"] > 1):
                        convitem["has_value"] = True

                    # TODO: python serialize to python - not json

                    # print(item)
                values[item["property_unique"]] = convitem

        retval = json.dumps(values, indent=4, ensure_ascii=False)
        retval = re.sub('(?<=\"measurement_type\": )(\d+)',
                        lambda i: helpers.hex2(int(i.group(1)), 2), retval)
        retval = re.sub('(?<=\"device_event_type\": )(\d+)',
                        lambda i: hex(int(i.group(1))), retval)
        # retval = re.sub(
        #     '(?<=: )(\d+)', lambda i: f"0x{int(i.group(0)):02x}", retval
        # )
        retval = retval.replace("true", "True")

        return retval

    fname = TARGET_BTHOMEBASE_DIR + "const_generated.py"
    print(f"generating {fname}...")
    f = open(fname, "w", encoding="utf-8")

    main_types_unique = sorted(imain for imain in set(main_types) if imain)
    for imain in main_types_unique:
        if imain is None:
            continue

        f.write(f"MEASUREMENT_TYPES_{imain.upper()}_SENSOR = ")
        data1 = generate_const(data, imain)
        f.write(data1)
        f.write("\n")

    f.close()


create_const_generated(data)

################################################################


def dump_types_for_doc(data):

    col_length = 30
    col_sep_char = " "

    def gen_header(achar, asepchar, data_columns):
        return asepchar.join([achar*(col_length)]*(len(data_columns))) + "\n"

    fname = TARGET_DOC_DIR + "bthome_common_format_generated.rst"
    print(f"generating {fname}...")
    f = open(fname, "w", encoding="utf-8")
    f.write(":orphan:\n\n")

    data_columns = ["Object id", "Property",
                    "Length (byte)", "Decimals", "Unit", "Type"]
    f.write(gen_header("=", " ", data_columns))
    f.write(col_sep_char.join(('{0: <'+str(col_length)+'}').format(col)
            for col in data_columns) + "\n")
    f.write(gen_header("-", " ", data_columns))

    data_columns = ["measurement_type_hex16", "property_unique",
                    "data_type_length", "accuracy_decimals", "unit_of_measurement", "main_type"]
    for item in data:
        f.write(col_sep_char.join(('{0: <'+str(col_length)+'}').format(
            item[col] or " ") for col in data_columns) + "\n")
    f.write(gen_header("=", " ", data_columns))

    f.close()


dump_types_for_doc(data)
