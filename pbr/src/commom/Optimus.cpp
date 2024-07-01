#if _WIN32
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL
#endif

// œ‘ø®—°‘Ò
extern "C" {
	EXPORT_SYMBOL int NvOptimusEnablement = 1;
	EXPORT_SYMBOL int AmdPowerXpressRequestHighPerformance = 1;
}
