// Der folgende ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem NATIVERENDERER_EXPORTS-Symbol
// (in der Befehlszeile definiert) kompiliert. Dieses Symbol darf für kein Projekt definiert werden,
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, sehen
// NATIVERENDERER_API-Funktionen als aus einer DLL importiert an, während diese DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.
#ifdef NATIVERENDERER_EXPORTS
#define NATIVERENDERER_API __declspec(dllexport)
#else
#define NATIVERENDERER_API __declspec(dllimport)
#endif

// Diese Klasse wird aus der DLL exportiert.

/*class NATIVERENDERER_API CNativeRenderer {
public:
	CNativeRenderer(void);
	// TODO: Methoden hier hinzufügen.
};

extern NATIVERENDERER_API int nNativeRenderer;

NATIVERENDERER_API int fnNativeRenderer(void);
*/

extern "C" {

	NATIVERENDERER_API int CreateRenderer(HWND hwnd);
	NATIVERENDERER_API void RenderFrame();
	NATIVERENDERER_API void ResizeRenderer(int width, int height);
	NATIVERENDERER_API void DestroyRenderer();

}
