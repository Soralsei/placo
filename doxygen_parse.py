import glob
import re
import xml.etree.ElementTree as ET

# Mapping member definitions (id) to all informations
member_definitions: dict = {}

# Compound members
compound_members: dict = {}
compound_metadata: dict = {}

# Mapping compound id to name
rewrite_types: dict = {
    "std::string": "str",
    "double": "float",
    "int": "int",
    "bool": "bool",
    "Eigen::MatrixXd": "numpy.ndarray",
    "Eigen::VectorXd": "numpy.ndarray",
    "Eigen::Matrix3d": "numpy.ndarray",
    "Eigen::Vector3d": "numpy.ndarray",
    "Eigen::Matrix2d": "numpy.ndarray",
    "Eigen::Vector2d": "numpy.ndarray",
    "Eigen::Affine3d": "numpy.ndarray",
    "void": "None",
}


def resolve_type(node):
    type_node = node.find("type")
    if type_node.find("ref") is not None:
        return type_node.find("ref").attrib["refid"]
    else:
        return type_node.text


def parse_compound(compounddef_node: ET.Element):
    global member_definitions, compound_metadata

    name = compounddef_node.find("compoundname").text
    compound_kind = compounddef_node.attrib["kind"]
    id = compounddef_node.attrib["id"]
    rewrite_types[id] = name
    compound_members[name] = []

    compound_metadata[name] = {
        "id": id,
        "kind": compound_kind,
        "name": name,
        "brief": compounddef_node.find("briefdescription/para").text
        if compounddef_node.find("briefdescription/para") is not None
        else None,
    }

    # Searching for member definitions
    for member in compounddef_node.findall("sectiondef/memberdef"):
        kind = member.attrib["kind"]
        id = member.attrib["id"]

        if kind in ["function", "variable", "enum", "namespace"]:
            member_definitions[id] = {
                "kind": kind,
                "name": member.find("name").text,
                "params": [],
            }
            member_definitions[id]["type"] = resolve_type(member)

            for param in member.findall("param"):
                param_type = resolve_type(param)
                param_name = None
                if param.find("declname") is not None:
                    param_name = param.find("declname").text
                if param.find("defname") is not None:
                    param_name = param.find("defname").text
                def_val = param.find("defval").text if param.find("defval") is not None else None
                member_definitions[id]["params"].append({"type": param_type, "name": param_name, "default": def_val})

            # Brief description
            brief = member.find("briefdescription/para")
            if brief is not None:
                member_definitions[id]["brief"] = brief.text

            # Detailed description
            member_definitions[id]["detailed"] = []
            for entry in member.findall("detaileddescription/para/parameterlist/parameteritem"):
                param_name = entry.find("parameternamelist/parametername").text
                param_desc = entry.find("parameterdescription/para").text
                member_definitions[id]["detailed"].append({"name": param_name, "desc": param_desc})

            # Verbatim
            verbatim = member.find("detaileddescription/para/verbatim")
            if verbatim is not None:
                member_definitions[id]["verbatim"] = verbatim.text

            # Return type
            return_type = member.find("detaileddescription/para/simplesect[@kind='return']")
            if return_type:
                member_definitions[id]["returns"] = return_type.find("para").text

            if compound_kind == "namespace":
                compound_members[name].append(id)

        elif kind == "typedef":
            rewrite_types[id] = resolve_type(member)

    # Listinf all members
    kind = compounddef_node.attrib["kind"]
    for member in compounddef_node.findall("listofallmembers/member"):
        compound_members[name].append(member.attrib["refid"])


def parse_xml(xml_file: str):
    with open(xml_file, "r") as f:
        xml_content = f.read()
        xml_content = ET.fromstring(xml_content)

        # Searching for compounddef nodes (classs & struct)
        for compounddef_node in xml_content.findall("compounddef"):
            parse_compound(compounddef_node)


def rewrite_type(typename: str):
    """
    We apply here some heuristics to rewrite C++ types to Python
    """
    if typename is not None:
        typename = typename.replace("&", "")
        typename = typename.replace("*", "")
        typename = typename.strip()
        if typename.startswith("const "):
            typename = typename[6:]

        if typename in rewrite_types:
            return rewrite_types[typename]
        else:
            if typename.startswith("std::vector"):
                return "list[" + rewrite_type(typename[12:-1]) + "]"

            return re.sub("[^a-zA-Z0-9\.]", "_", typename)

    return None


def parse_directory(directory):
    global member_definitions, compound_members
    doxygen_xml_files = glob.glob(f"{directory}/xml/*.xml")

    for xml_file in doxygen_xml_files:
        parse_xml(xml_file)

    # Resolving types
    for member in member_definitions:
        member_definition = member_definitions[member]
        member_definition["type"] = rewrite_type(member_definition["type"])

        for param in member_definition["params"]:
            param["type"] = rewrite_type(param["type"])

    # Resolving compound members
    for name in compound_members:
        temp = compound_members[name]
        compound_members[name] = {}
        for id in temp:
            if id in member_definitions:
                compound_members[name][member_definitions[id]["name"]] = member_definitions[id]


def get_members(name: str):
    if name not in compound_members:
        return None

    return compound_members[name]


def get_metadata(name: str):
    if name not in compound_metadata:
        return None

    return compound_metadata[name]