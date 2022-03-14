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

using namespace mediasoupclient;

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

	DLL_EXPORT const nlohmann::json* GetSctpCapabilities(mediasoupclient::Device* device)
	{
		return &device->GetSctpCapabilities();
	}

	DLL_EXPORT const nlohmann::json* GetRtpCapabilities(mediasoupclient::Device* device)
	{
		return &device->GetRtpCapabilities();
	}

	DLL_EXPORT bool IsLoaded(mediasoupclient::Device* device)
	{
		return device->IsLoaded();
	}

	DLL_EXPORT void DeviceLoad(Device* device, const nlohmann::json* rtpCapabilities, const PeerConnection::Options* peerConnectionOptions = nullptr)
	{
		device->Load(*rtpCapabilities, peerConnectionOptions);
	}

	DLL_EXPORT void CleanUp()
	{
		mediasoupclient::Cleanup();
	}

	DLL_EXPORT void __stdcall Version(char* text, size_t bufferSize)
	{
		//C# CallingConvention.stdCall
		strcpy_s(text, bufferSize, mediasoupclient::Version().c_str());
	}

	
}