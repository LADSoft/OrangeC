#include "PEFile.h"
#include <iostream>
#include <sstream>
using namespace DotNetPELib;


static PEReader r;
std::string StringOf(String index)
{
    Byte buf[512];
    r.ReadFromString(buf, sizeof(buf), index.index_);
    return (char *)buf;
}
std::string GUIDOf(GUID index)
{
    Byte buf[16];
    r.ReadFromGUID(buf, sizeof(buf), index.index_);
    std::stringstream stream;

    for (int i= 3; i >= 0; i--)
    {
        stream.width(2);
        stream.fill('0');
        stream << std::hex << (int)buf[i];
    }
    stream << "-";
    for (int i= 5; i >= 4; i--)
    {
        stream.width(2);
        stream.fill('0');
        stream << std::hex << (int)buf[i];
    }
    stream << "-";
    for (int i= 7; i >= 6; i--)
    {
        stream.width(2);
        stream.fill('0');
        stream << std::hex << (int)buf[i];
    }
    stream << "-";
    for (int i= 8; i <= 9; i++)
    {
        stream.width(2);
        stream.fill('0');
        stream << std::hex << (int)buf[i];
    }
    stream << "-";
    for (int i= 10; i <= 15; i++)
    {
        stream.width(2);
        stream.fill('0');
        stream << std::hex << (int)buf[i];
    }
    char cbuf[256];
    stream.getline(cbuf, 256);
    return cbuf;
}
void DumpModule(const ModuleTableEntry& table)
{
    std::cout << "Module: " << StringOf(table.nameIndex_) << std::endl;
    std::cout << "    GUID:" << GUIDOf(table.guidIndex_) << std::endl;
}
void DumpTypeRef(const TypeRefTableEntry& table)
{
    if (table.resolution_.tag_ == 2)
	std::cout << "TypeRef: " << "[" << table.resolution_.index_ << "]" << StringOf(table.typeNameSpaceIndex_) << " " << StringOf(table.typeNameIndex_)<< std::endl;
    else
	std::cout << "TypeRef: " << StringOf(table.typeNameSpaceIndex_) << " " << StringOf(table.typeNameIndex_)<< std::endl;

}
void DumpTypeDef(const TypeDefTableEntry& table)
{
    std::cout << "TypeDef: " << std::hex << table.flags_ << " " <<StringOf(table.typeNameSpaceIndex_) << " " << StringOf(table.typeNameIndex_)<< std::endl;
}
void DumpField(const FieldTableEntry& table)
{
    std::cout << "Field: " << std::hex << table.flags_ << " " << StringOf(table.nameIndex_)<< std::endl;
}
void DumpMethodDef(const MethodDefTableEntry& table)
{
    std::cout << "MethodDef: " << std::hex << table.flags_ << " " << std::hex << table.implFlags_ << " " << std::hex<< table.rva_ << " " << StringOf(table.nameIndex_)<< std::endl;
}
void DumpParam(const ParamTableEntry& table)
{
    std::cout << "Param: " << std::hex << table.flags_ << " " << std::hex << table.sequenceIndex_ << " " << StringOf(table.nameIndex_)<< std::endl;
}
void DumpInterfaceImpl(const InterfaceImplTableEntry& table)
{
    std::cout << "InterfaceImpl" << std::endl;
}
void DumpMemberRef(const MemberRefTableEntry& table)
{
    std::cout << "MemberRef: " << StringOf(table.nameIndex_) << std::endl;
}
void DumpConstant(const ConstantTableEntry& table)
{
}
void DumpCustomAttribute(const CustomAttributeTableEntry& table)
{
}
void DumpFieldMarshal(const FieldMarshalTableEntry& table)
{
}
void DumpDeclSecurity(const DeclSecurityTableEntry& table)
{
}
void DumpClassLayout(const ClassLayoutTableEntry& table)
{
}
void DumpFieldLayout(const FieldLayoutTableEntry& table)
{
}
void DumpStandaloneSig(const StandaloneSigTableEntry& table)
{
}
void DumpEventMap(const EventMapTableEntry& table)
{
}
void DumpEvent(const EventTableEntry& table)
{
}
void DumpPropertyMap(const PropertyMapTableEntry& table)
{
}
void DumpProperty(const PropertyTableEntry& table)
{
}
void DumpMethodSemantics(const MethodSemanticsTableEntry& table)
{
}
void DumpMethodImpl(const MethodImplTableEntry& table)
{
}
void DumpModuleRef(const ModuleRefTableEntry& table)
{
}
void DumpTypeSpec(const TypeSpecTableEntry& table)
{
}
void DumpImplMap(const ImplMapTableEntry& table)
{
}
void DumpFieldRVA(const FieldRVATableEntry& table)
{
}
void DumpAssemblyDef(const AssemblyDefTableEntry& table)
{
    std::cout << "AssemblyDef: " << table.major_ << " " << table.minor_ << " " << table.build_ << " " << table.revision_ << std::endl;
}
void DumpAssemblyRef(const AssemblyRefTableEntry& table)
{
    std::cout << "AssemblyRef: " << StringOf(table.nameIndex_) << " " << table.major_ << " " << table.minor_ << " " << table.build_ << " " << table.revision_ << std::endl;
}
void DumpFile(const FileTableEntry& table)
{
}
void DumpExportedType(const ExportedTypeTableEntry& table)
{
}
void DumpManifestResource(const ManifestResourceTableEntry& table)
{
}
void DumpNestedClass(const NestedClassTableEntry& table)
{
}
void DumpGenericParam(const GenericParamTableEntry& table)
{
}
void DumpMethodSpec(const MethodSpecTableEntry& table)
{
}
void DumpGenericParamConstraints(const GenericParamConstraintsTableEntry& table)
{
}
void Dump(int i, const TableEntryBase &table)
{
    switch (i)
    {
        case tModule:
            DumpModule(static_cast<const ModuleTableEntry&>(table));
            break;
        case tTypeRef:
            DumpTypeRef(static_cast<const TypeRefTableEntry&>(table));
            break;
        case tTypeDef:
            DumpTypeDef(static_cast<const TypeDefTableEntry&>(table));
            break;
        case tField:
            DumpField(static_cast<const FieldTableEntry&>(table));
            break;
        case tMethodDef:
            DumpMethodDef(static_cast<const MethodDefTableEntry&>(table));
            break;
        case tParam:
            DumpParam(static_cast<const ParamTableEntry&>(table));
            break;
        case tInterfaceImpl:
            DumpInterfaceImpl(static_cast<const InterfaceImplTableEntry&>(table));
            break;
        case tMemberRef:
            DumpMemberRef(static_cast<const MemberRefTableEntry&>(table));
            break;
        case tConstant:
            DumpConstant(static_cast<const ConstantTableEntry&>(table));
            break;
        case tCustomAttribute:
            DumpCustomAttribute(static_cast<const CustomAttributeTableEntry&>(table));
            break;
        case tFieldMarshal:
            DumpFieldMarshal(static_cast<const FieldMarshalTableEntry&>(table));
            break;
        case tDeclSecurity:
            DumpDeclSecurity(static_cast<const DeclSecurityTableEntry&>(table));
            break;
        case tClassLayout:
            DumpClassLayout(static_cast<const ClassLayoutTableEntry&>(table));
            break;
        case tFieldLayout:
            DumpFieldLayout(static_cast<const FieldLayoutTableEntry&>(table));
            break;
        case tStandaloneSig:
            DumpStandaloneSig(static_cast<const StandaloneSigTableEntry&>(table));
            break;
        case tEventMap :
            DumpEventMap(static_cast<const EventMapTableEntry&>(table));
            break;
        case tEvent :
            DumpEvent(static_cast<const EventTableEntry&>(table));
            break;
        case tPropertyMap:
            DumpPropertyMap(static_cast<const PropertyMapTableEntry&>(table));
            break;
        case tProperty:
            DumpProperty(static_cast<const PropertyTableEntry&>(table));
            break;
        case tMethodSemantics:
            DumpMethodSemantics(static_cast<const MethodSemanticsTableEntry&>(table));
            break;
        case tMethodImpl:
            DumpMethodImpl(static_cast<const MethodImplTableEntry&>(table));
            break;
        case tModuleRef:
            DumpModuleRef(static_cast<const ModuleRefTableEntry&>(table));
            break;
        case tTypeSpec:
            DumpTypeSpec(static_cast<const TypeSpecTableEntry&>(table));
            break;
        case tImplMap:
            DumpImplMap(static_cast<const ImplMapTableEntry&>(table));
            break;
        case tFieldRVA:
            DumpFieldRVA(static_cast<const FieldRVATableEntry&>(table));
            break;
        case tAssemblyDef:
            DumpAssemblyDef(static_cast<const AssemblyDefTableEntry&>(table));
            break;
        case tAssemblyRef:
            DumpAssemblyRef(static_cast<const AssemblyRefTableEntry&>(table));
            break;
        case tFile:
            DumpFile(static_cast<const FileTableEntry&>(table));
            break;
        case tExportedType:
            DumpExportedType(static_cast<const ExportedTypeTableEntry&>(table));
            break;
        case tManifestResource:
            DumpManifestResource(static_cast<const ManifestResourceTableEntry&>(table));
            break;
        case tNestedClass:
            DumpNestedClass(static_cast<const NestedClassTableEntry&>(table));
            break;
        case tGenericParam:
            DumpGenericParam(static_cast<const GenericParamTableEntry&>(table));
            break;
        case tMethodSpec:
            DumpMethodSpec(static_cast<const MethodSpecTableEntry&>(table));
            break;
        case tGenericParamConstraint:
            DumpGenericParamConstraints(static_cast<const GenericParamConstraintsTableEntry&>(table));
            break;
    }
}
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        int n = r.ManagedLoad(argv[1],0,0,0,0);
        if (!n)
        {
            for (int i=0; i < MaxTables; i++)
            {
                const DNLTable &table = r.Table(i);
                if (table.size())
                    for (int j=0; j < table.size(); j++)
                        Dump(i, *(table[j]));
            }
        }
        else
        {
            printf("error: %d\n", n);
        }
    }
}