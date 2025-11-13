#pragma once
#include "ppCommon.h"
#include <functional>
#include <memory>
#include "UTF8.h"
// This class creates a separation of concern from the templated classes vs the non-templated classes
// This effectively is a static instance we can deal with
class TokenizerSettings
{
  public:
    TokenizerSettings() : dialect(Dialect::c23), disableExtensions(false) {}
    using symbol_check_function = bool(const char*, bool);

  private:
    std::function<symbol_check_function> internal_check_func = TokenizerSettings::IsSymbolCharDefault;
    static std::shared_ptr<TokenizerSettings> instance;
    bool disableExtensions;
    Dialect dialect;

  public:
    static bool IsSymbolCharDefault(const char* data, bool startOnly)
    {
        switch (*data)
        {
            case '_':
            case '$':
                return true;
            default:
                return (startOnly ? UTF8::IsAlpha(data) : UTF8::IsAlnum(data));
        }
    }

    static std::shared_ptr<TokenizerSettings> Instance()
    {
        if (!instance)
            instance = std::shared_ptr<TokenizerSettings>(new TokenizerSettings());
        return instance;
    }

    bool GetExtensionsDisabled() { return disableExtensions; }
    Dialect GetDialect() { return dialect; }
    void SetDialect(Dialect dialect) { this->dialect = dialect; }
    void SetExtensionsDisabled(bool disableExtension) { this->disableExtensions = disableExtension; }
    void SetSymbolCheckFunction(std::function<symbol_check_function> func) { this->internal_check_func = func; }
    std::function<symbol_check_function> GetSymbolCheckFunction() { return this->internal_check_func; }
};