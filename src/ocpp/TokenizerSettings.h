#pragma once
#include "ppCommon.h"
#include <memory>
// This class creates a separation of concern from the templated classes vs the non-templated classes, nothing relies on this (yet)
// but this transition is instrumental in ensuring that we can deal with header-only compile-time vs non-compile-time attributes
// that have to be dealt with globally

class TokenizerSettings
{
  private:
    static std::shared_ptr<TokenizerSettings> instance;
    bool disableExtensions;
    Dialect dialect;

  public:
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
};