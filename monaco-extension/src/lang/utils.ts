import * as Monaco from 'monaco-editor';

export type IMonaco = typeof Monaco;

export function languageIsInitialized(monaco: IMonaco, langId: string) {
    return monaco.languages.getLanguages().some(lang => lang.id === langId);
}

export function registerLanguage(
    monaco: IMonaco,
    langId: string,
    tokensProvider: Monaco.languages.IMonarchLanguage,
    config: Monaco.languages.LanguageConfiguration,
) {
    if (languageIsInitialized(monaco, langId)) {
        return;
    }

    monaco.languages.register({
        id: langId,
    });
    monaco.languages.setMonarchTokensProvider(langId, tokensProvider);
    monaco.languages.setLanguageConfiguration(langId, config);
}
