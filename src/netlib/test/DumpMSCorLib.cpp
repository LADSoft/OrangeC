// this is an example of how to use traversal to dump the public members of a .net assembly
// instead of dumping them you could load them into a compiler's symbol table, for example..
#include "DotNetPELib.h"

#include <fstream>
#include <deque>
#include <string>
using namespace DotNetPELib;

class Importer :public Callback
{
public:
    Importer() { }
    virtual ~Importer() { }

    virtual bool EnterAssembly(const AssemblyDef *) override;
    virtual bool ExitAssembly(const AssemblyDef *) override;
    virtual bool EnterNamespace(const Namespace *) override;
    virtual bool ExitNamespace(const Namespace *) override;
    virtual bool EnterClass(const Class *) override;
    virtual bool ExitClass(const Class *) override;
    virtual bool EnterEnum(const Enum *cls) override;
    virtual bool ExitEnum(const Enum *cls) override;
    virtual bool EnterMethod(const Method *) override;
    virtual bool EnterField(const Field *) override;
    virtual bool EnterProperty(const Property *) override;

    void diag(std::string header, std::string name)
    {
        
        for (int i = 0; i < names_.size(); i++)
            std::cout << "\t";
        if (header.size())
            std::cout << header << " ";
        for (int i = 0; i < names_.size(); i++)
            std::cout << names_[i] << ".";
        std::cout << name << std::endl;
    }
private:
    std::deque<std::string > names_;
};

void main()
{
    PELib lib("test", PELib::ilonly);

    if (!lib.LoadAssembly("mscorlib"))
    {
        Importer importer;
        lib.Traverse(importer);
    }
}


bool Importer::EnterAssembly(const AssemblyDef *assembly)
{
    diag("Assembly", assembly->Name());
    names_.push_back(assembly->Name());
    return true;
}
bool Importer::ExitAssembly(const AssemblyDef *assembly) 
{
    names_.pop_back();
    diag("Exit Assembly", assembly->Name());
    return true;
}
bool Importer::EnterNamespace(const Namespace *nameSpace)
{
    diag("Namespace", nameSpace->Name());
    names_.push_back(nameSpace->Name());
    return true;
}
bool Importer::ExitNamespace(const Namespace *nameSpace)
{
    names_.pop_back();
    diag("Exit Namespace", nameSpace->Name());
    return true;
}
bool Importer::EnterClass(const Class *cls)
{
    diag("Class", cls->Name());
    names_.push_back(cls->Name());
    return true;
}
bool Importer::ExitClass(const Class *cls)
{
    names_.pop_back();
    diag("Exit Class", cls->Name());
    return true;
}
bool Importer::EnterEnum(const Enum *cls)
{
    diag("Enum", cls->Name());
    names_.push_back(cls->Name());
    return true;
}
bool Importer::ExitEnum(const Enum *cls)
{
    names_.pop_back();
    diag("Exit Enum", cls->Name());
    return true;
}
bool Importer::EnterMethod(const Method *method)
{
    diag("Method", method->Signature()->Name());
    return true;
}
bool Importer::EnterField(const Field *field)
{
    diag("Field", field->Name());
    return true;
}
bool Importer::EnterProperty(const Property *property)
{
    diag("Property", property->Name());
    return true;
}