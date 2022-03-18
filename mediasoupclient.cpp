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
	//��� �� �� �غ����ҵ� cpp event�� � ������ �����ؾ�����
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

	//Check Pointer return �Ҵ��ؼ� �����ִ��� �����Ϳ� �����ϴ��� üũ�� �ʿ� ���� �ҽ��ڵ� ����
	DLL_EXPORT DataConsumer* ConsumeData(
		RecvTransport* recvTransport,
		DataConsumer::Listener* listener,
		const std::string& id,
		const std::string& producerId,
		const uint16_t streamId,
		const std::string& label,
		const std::string& protocol = std::string(),
		const nlohmann::json& appData = nlohmann::json::object()
		)
	{
		if (recvTransport == nullptr || listener == nullptr)
			return nullptr;
		recvTransport->ConsumeData(listener, id, producerId, streamId, label, protocol, appData);
	}
#pragma endregion

#pragma region Producer
	DLL_EXPORT const string& GetId(Producer* producer)
	{
		if (producer == nullptr)
			return "";
		return producer->GetId();
	}
	
	DLL_EXPORT string GetKind(Producer* producer)
	{
		if (producer == nullptr)
			return "";
		return producer->GetKind();//Return audio or video
	}

	DLL_EXPORT webrtc::MediaStreamTrackInterface* GetTrack(Producer* producer)
	{
		if (producer == nullptr)
			return nullptr;
		return producer->GetTrack();
	}

	DLL_EXPORT const nlohmann::json& GetRtpParameters(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();
		return producer->GetRtpParameters();
	}

	DLL_EXPORT const uint8_t GetMaxSpatialLayer(Producer* producer)
	{
		if (producer == nullptr)
			return 0;
		return producer->GetMaxSpatialLayer();
	}

	DLL_EXPORT nlohmann::json GetStats(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();
		return producer->GetStats();
	}

	DLL_EXPORT const nlohmann::json& GetAppData(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();
		return producer->GetAppData();
	}

	DLL_EXPORT bool IsClosed(Producer* producer)
	{
		if (producer == nullptr)
			return true;
		return producer->IsClosed();
	}

	DLL_EXPORT bool IsPaused(Producer* producer)
	{
		if (producer == nullptr)
			return false;
		return producer->IsPaused();
	}

	DLL_EXPORT void Close(Producer* producer)
	{
		if (producer == nullptr)
			return;
		producer->Close();
	}

	DLL_EXPORT void Pause(Producer* producer)
	{
		if (producer == nullptr)
			return;
		producer->Pause();
	}

	DLL_EXPORT void Resume(Producer* producer)
	{
		if (producer == nullptr)
			return;
		producer->Resume();
	}

	DLL_EXPORT void ReplaceTrack(Producer* producer, webrtc::MediaStreamTrackInterface* track)
	{
		if (producer == nullptr || track == nullptr)
			return;
		producer->ReplaceTrack(track);
	}

	DLL_EXPORT void SetMaxSpatialLayer(Producer* producer, uint8_t spatialLayer)
	{
		if (producer == nullptr)
			return;
		producer->SetMaxSpatialLayer(spatialLayer);
	}
#pragma endregion

#pragma region Consumer
	DLL_EXPORT const string& GetId(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";
		return consumer->GetId();
	}

	DLL_EXPORT const string& GetProducerId(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";
		return consumer->GetProducerId();
	}

	DLL_EXPORT string GetKind(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";
		return consumer->GetKind();//Return audio or video
	}

	DLL_EXPORT webrtc::MediaStreamTrackInterface* GetTrack(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nullptr;
		return consumer->GetTrack();
	}

	DLL_EXPORT const nlohmann::json& GetRtpParameters(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();
		return consumer->GetRtpParameters();
	}

	DLL_EXPORT nlohmann::json GetStats(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();
		return consumer->GetStats();
	}

	DLL_EXPORT const nlohmann::json& GetAppData(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();
		return consumer->GetAppData();
	}

	DLL_EXPORT bool IsClosed(Consumer* consumer)
	{
		if (consumer == nullptr)
			return true;
		return consumer->IsClosed();
	}

	DLL_EXPORT bool IsPaused(Consumer* consumer)
	{
		if (consumer == nullptr)
			return false;
		return consumer->IsPaused();
	}

	DLL_EXPORT void Close(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		consumer->Close();
	}

	DLL_EXPORT void Pause(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		consumer->Pause();
	}

	DLL_EXPORT void Resume(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		consumer->Resume();
	}
#pragma endregion
	
#pragma region DataProducer
	DLL_EXPORT const string& GetId(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return "";
		return dataProducer->GetId();
	}

	DLL_EXPORT const nlohmann::json& GetSctpStreamParameters (DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return nlohmann::json::object();
		return dataProducer->GetSctpStreamParameters();
	}

	DLL_EXPORT webrtc::DataChannelInterface::DataState GetReadyState(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  webrtc::DataChannelInterface::DataState::kClosed;
		return dataProducer->GetReadyState();
	}

	DLL_EXPORT const string& GetLabel(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";
		return dataProducer->GetLabel();
	}

	DLL_EXPORT const string& GetProtocol(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";
		return dataProducer->GetProtocol();
	}

	DLL_EXPORT const uint8_t GetBufferedAmount(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  -1;
		return dataProducer->GetBufferedAmount();
	}

	DLL_EXPORT const nlohmann::json& GetAppData(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  nlohmann::json::object();
		return dataProducer->GetAppData();
	}

	DLL_EXPORT bool IsClosed(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return true;
		return dataProducer->IsClosed();
	}

	DLL_EXPORT void Close(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return;
		dataProducer->Close();
	}

	DLL_EXPORT void Send(DataProducer* dataProducer, webrtc::DataBuffer& buffer)
	{
		if (dataProducer == nullptr)
			return;
		dataProducer->Send(buffer);
	}
#pragma endregion

#pragma region DataConsumer
	DLL_EXPORT const string& GetId(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return "";
		return dataConsumer->GetId();
	}

	DLL_EXPORT const string& GetId(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return "";
		return dataConsumer->GetDataProducerId();
	}

	DLL_EXPORT const nlohmann::json& GetSctpStreamParameters(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return nlohmann::json::object();
		return dataConsumer->GetSctpStreamParameters();
	}

	DLL_EXPORT webrtc::DataChannelInterface::DataState GetReadyState(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return  webrtc::DataChannelInterface::DataState::kClosed;
		return dataConsumer->GetReadyState();
	}

	DLL_EXPORT const string& GetLabel(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return  "";
		return dataConsumer->GetLabel();
	}

	DLL_EXPORT const string& GetProtocol(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return  "";
		return dataConsumer->GetProtocol();
	}

	DLL_EXPORT const nlohmann::json& GetAppData(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return  nlohmann::json::object();
		return dataConsumer->GetAppData();
	}

	DLL_EXPORT bool IsClosed(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return true;
		return dataConsumer->IsClosed();
	}

	DLL_EXPORT void Close(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return;
		dataConsumer->Close();
	}

	DLL_EXPORT void Send(DataProducer* dataProducer, webrtc::DataBuffer& buffer)
	{
		if (dataProducer == nullptr)
			return;
		dataProducer->Send(buffer);
	}
#pragma endregion

#pragma region Logger

#pragma endregion
}