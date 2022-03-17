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

///<summary>
/// check out below may help you understand
/// https://mediasoup.org/documentation/v3/libmediasoupclient/api
///</summary

extern "C"
{
#pragma region mediasoup
	DLL_EXPORT void Initialize()
	{
		mediasoupclient::Initialize();
	}

	DLL_EXPORT void CleanUp()
	{
		mediasoupclient::Cleanup();
	}

	DLL_EXPORT void __stdcall Version(char* text, size_t bufferSize)
	{
		//C# CallingConvention.stdCall
		if (text == nullptr)
			return;
		strcpy_s(text, bufferSize, mediasoupclient::Version().c_str());
	}
#pragma endregion

#pragma region Device
	DLL_EXPORT mediasoupclient::Device* MakeDevice()
	{
		return new mediasoupclient::Device();
	}

	DLL_EXPORT const nlohmann::json* GetSctpCapabilities(mediasoupclient::Device* device)
	{
		if (device == nullptr)
			return nullptr;
		return &device->GetSctpCapabilities();
	}

	DLL_EXPORT const nlohmann::json* GetRtpCapabilities(mediasoupclient::Device* device)
	{
		if (device == nullptr)
			return nullptr;
		return &device->GetRtpCapabilities();
	}

	DLL_EXPORT bool IsLoaded(Device* device)
	{
		if (device == nullptr)
			return false;
		return device->IsLoaded();
	}

	DLL_EXPORT void DeviceLoad(Device* device, const nlohmann::json* rtpCapabilities, const PeerConnection::Options* peerConnectionOptions = nullptr)
	{
		if (device == nullptr || rtpCapabilities == nullptr)
			return;
		device->Load(*rtpCapabilities, peerConnectionOptions);
	}

	DLL_EXPORT bool CanProduce(Device* device, const char* type)
	{
		if (device == nullptr)
			return false;
		return device->CanProduce(type);
	}


	//TODO wrapping needed Overload exist
	DLL_EXPORT SendTransport* CreateSendTransport(Device* device, SendTransport::Listener* listener,
		const std::string& id,
		const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters,
		const PeerConnection::Options* peerConnectionOptions = nullptr,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		if (device == nullptr || listener == nullptr)
			return nullptr;

		SendTransport* transport = device->CreateSendTransport(listener, iceParameters, iceCandidates, dtlsParameters, sctpParameters, peerConnectionOptions, appData);
		return transport;
	}

	//TODO wrapping needed Overload exist
	DLL_EXPORT RecvTransport* CreateRecvTransport(
		Device* device,
		RecvTransport::Listener* listener,
		const std::string& id,
		const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters,
		const PeerConnection::Options* peerConnectionOptions = nullptr,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		if (device == nullptr || listener == nullptr)
			return nullptr;

		RecvTransport* transport = device->CreateRecvTransport(listener, id, iceParameters, iceCandidates, dtlsParameters, sctpParameters, peerConnectionOptions, appData);
		return transport;
	}
#pragma endregion

#pragma region Transport
	//Transport
	//TODO change string to char for C# stringbuilder
	DLL_EXPORT const string GetId(Transport* transport)
	{
		if (transport == nullptr)
			return nullptr;
		return transport->GetId();
	}

	DLL_EXPORT const string GetConnectionState(Transport* transport)
	{
		if (transport == nullptr)
			return nullptr;
		return transport->GetConnectionState();
	}

	DLL_EXPORT bool IsClosed(Transport* transport)
	{
		if (transport == nullptr)
			return true;

		return transport->IsClosed();
	}


	DLL_EXPORT const nlohmann::json GetStats(Transport* transport)
	{
		if (transport == nullptr)
			return true;

		return transport->GetStats();
	}

	//API comment : This method should be called when the server side transport has been closed (and vice-versa)
	DLL_EXPORT void Close(Transport* transport)
	{
		if (transport == nullptr)
			return;

		transport->Close();
	}

	DLL_EXPORT void RestartIce(Transport* transport, const nlohmann::json& iceParameters)
	{
		if (transport == nullptr)
			return;

		transport->RestartIce(iceParameters);
	}

	DLL_EXPORT void UpdateIceServers(Transport* transport, const nlohmann::json& iceServers = nlohmann::json())
	{
		if (transport == nullptr)
			return;

		transport->UpdateIceServers(iceServers);
	}

	//Transport Event
	//고민 좀 더 해봐야할듯 cpp event는 어떤 식으로 래핑해야할지
#pragma endregion

#pragma region SendTransport
	DLL_EXPORT Producer* Produce(
		SendTransport* sendTransport,
		Producer::Listener* producerListener,
		webrtc::MediaStreamTrackInterface* track,
		const std::vector<webrtc::RtpEncodingParameters>* encodings,
		const nlohmann::json* codecOptions,
		const nlohmann::json* codec,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		if (sendTransport == nullptr || producerListener == nullptr)
			return nullptr;

		return sendTransport->Produce(producerListener, track, encodings, codecOptions, codec, appData);
	}

	DLL_EXPORT DataProducer* ProduceData(
		SendTransport* sendTransport,
		DataProducer::Listener* listener,
		const std::string& label = "",
		const std::string& protocol = "",
		bool ordered = true,
		int maxRetransmits = 0,
		int maxPacketLifeTime = 0,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		return sendTransport->ProduceData(listener, label, protocol, ordered, maxRetransmits, maxPacketLifeTime, appData);
	}

	//SendTransport Listener
#pragma endregion

#pragma region RectTransport
	DLL_EXPORT Consumer* Consume(
		RecvTransport* recvTransport,
		Consumer::Listener* consumerListener,
		const std::string& id,
		const std::string& producerId,
		const std::string& kind,
		nlohmann::json* rtpParameters,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		if (recvTransport == nullptr || consumerListener == nullptr)
			return nullptr;
		return recvTransport->Consume(consumerListener, id, producerId, kind, rtpParameters, appData);
	}
#pragma endregion

}