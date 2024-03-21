export function languageIsInitialized(monaco, langId) {
    return monaco.languages.getLanguages().some(lang => lang.id === langId);
}

export function registerLanguage(monaco, langId, tokensProvider, config) {
    // if (languageIsInitialized(monaco, langId)) {
    //     return;
    // }

    monaco.languages.register({
        id: langId,
    });
    monaco.languages.setMonarchTokensProvider(langId, tokensProvider);
    monaco.languages.setLanguageConfiguration(langId, config);
}
