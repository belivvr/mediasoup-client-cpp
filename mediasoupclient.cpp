#define WEBRTC_WIN
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "mediasoupclient.hpp"
using namespace std;

#define DLL_EXPORT_FLAG

#ifdef DLL_EXPORT_FLAG
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
#include <atlbase.h>
#include <atlconv.h>

extern "C"
{
	DLL_EXPORT void Initialize()
	{
		mediasoupclient::Initialize();
	}

	DLL_EXPORT mediasoupclient::Device* MakeDevice()
	{
		return new mediasoupclient::Device();
	}

	DLL_EXPORT const nlohmann::json& GetSctpCapabilities(mediasoupclient::Device* device)
	{
		return device->GetSctpCapabilities();
	}

	DLL_EXPORT const nlohmann::json& GetRtpCapabilities(mediasoupclient::Device* device)
	{
		return device->GetRtpCapabilities();
	}

	DLL_EXPORT bool IsLoaded(mediasoupclient::Device* device)
	{
		return device->IsLoaded();
	}

	DLL_EXPORT void CleanUp()
	{
		mediasoupclient::Cleanup();
	}

	DLL_EXPORT LPWSTR Version()
	{
		string* versionText = new string(mediasoupclient::Version());
		string* testString = new string("테스트 문자열인데 이건 어떻게 되나요?");
		delete versionText;
		LPWSTR test = (LPWSTR)testString->c_str();
		vector<wchar_t> data(wcslen(test) + 1);
		wcscpy_s(data.data(), data.size(), test);
		auto* result = data.data();
		return result;
	}
}