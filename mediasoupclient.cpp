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

const string currentDateTime();
void ErrorLogging(exception e, string prefix="");

///<summary>
/// check out below may help you understand
/// https://mediasoup.org/documentation/v3/libmediasoupclient/api
///</summary

extern "C"
{
#pragma region mediasoup
	DLL_EXPORT void Initialize()
	{
		try
		{
			mediasoupclient::Initialize();
		}
		catch (exception e)
		{
			ErrorLogging(e);
		}
	}

	DLL_EXPORT void CleanUp()
	{
		mediasoupclient::Cleanup();
	}

	//C# CallingConvention.stdCall
	// C++, C# string Marthaling
	//https://docs.microsoft.com/ko-kr/dotnet/api/system.runtime.interopservices.callingconvention?view=net-6.0
	DLL_EXPORT void __stdcall Version(char* text, size_t bufferSize)
	{
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

		const nlohmann::json* resultPtr = nullptr;
		try
		{
			resultPtr = &device->GetSctpCapabilities();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Device.GetSctpCapabilities]");
			return (const nlohmann::json*)-1;
		}

		return resultPtr;
	}

	DLL_EXPORT const nlohmann::json* GetRtpCapabilities(mediasoupclient::Device* device)
	{
		if (device == nullptr)
			return nullptr;
		nlohmann::json* rtp = nullptr;
		try
		{
			rtp = (nlohmann::json*)&device->GetRtpCapabilities();
		}
		catch(exception e)
		{
			ErrorLogging(e, "[Device.GetRtpCapabilities]");
			return (const nlohmann::json *)-1;
		}

		return rtp;
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
		try
		{
			device->Load(*rtpCapabilities, peerConnectionOptions);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Device.Load]");
		}
	}

	DLL_EXPORT bool CanProduce(Device* device, const char* type)
	{
		if (device == nullptr)
			return false;
		return device->CanProduce(type);
	}


	//TODO wrapping needed Overload exist
	DLL_EXPORT SendTransport* CreateSendTransport(Device* device, SendTransport::Listener* listener,
		const char* id,
		const nlohmann::json* iceParameters,
		const nlohmann::json* iceCandidates,
		const nlohmann::json* dtlsParameters,
		const nlohmann::json* sctpParameters,
		const PeerConnection::Options* peerConnectionOptions = nullptr,
		const nlohmann::json* appData = nullptr)
	{
		if (device == nullptr || listener == nullptr)
			return nullptr;
		SendTransport* transport = nullptr;
		
		try
		{
			string Id(id);
			const nlohmann::json iceParameter = *iceParameters;
			const nlohmann::json iceCandidate = *iceCandidates;
			const nlohmann::json dtlsParameter = *dtlsParameters;
			const nlohmann::json sctpParameter = *sctpParameters;
			const nlohmann::json data = appData == nullptr ? nlohmann::json::object() : *appData;
			transport = device->CreateSendTransport(listener, Id, iceParameter, iceCandidate, dtlsParameter, sctpParameter, peerConnectionOptions, data);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Device.CreateSendTransport]");
			return (SendTransport*)-1;
		}

		return transport;
	}

	DLL_EXPORT RecvTransport* CreateRecvTransport(
		Device* device,
		RecvTransport::Listener* listener,
		const char* id,
		const nlohmann::json* iceParameters,
		const nlohmann::json* iceCandidates,
		const nlohmann::json* dtlsParameters,
		const nlohmann::json* sctpParameters,
		const PeerConnection::Options* peerConnectionOptions = nullptr,
		const nlohmann::json* appData = nullptr)
	{
		if (device == nullptr || listener == nullptr)
			return nullptr;

		RecvTransport* transport = nullptr;
		try
		{
			string Id(id);
			const nlohmann::json iceParameter = *iceParameters;
			const nlohmann::json iceCandidate = *iceCandidates;
			const nlohmann::json dtlsParameter = *dtlsParameters;
			const nlohmann::json sctpParameter = *sctpParameters;
			const nlohmann::json data = appData == nullptr ? nlohmann::json::object() : *appData;
			transport = device->CreateRecvTransport(listener, Id, iceParameter, iceCandidate, dtlsParameter, sctpParameter, peerConnectionOptions, data);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Device.CreateRecvTransport]");
			return (RecvTransport*)-1;
		}
		
		return transport;
	}
#pragma endregion

#pragma region Transport
	//Transport
	//TODO change string to char for C# stringbuilder
	DLL_EXPORT const char* GetId(Transport* transport)
	{
		if (transport == nullptr)
			return "transport is null";
		string id;
		try
		{
			id = transport->GetId();
		}
		catch(exception e)
		{
			ErrorLogging(e, "[Transport.GetId]");
			return "[Transport]GetId Error";
		}
		const char* idPtr = id.c_str();

		return idPtr;
	}

	DLL_EXPORT const char* GetConnectionState(Transport* transport)
	{
		if (transport == nullptr)
			return "transport is null";

		string state;
		try
		{
			state = transport->GetConnectionState();
		}
		catch(exception e)
		{
			ErrorLogging(e, "[Transport.GetConnectionState]");
			state = "Error Occurred";
		}

		const char* stateContainer = state.c_str();
		
		return stateContainer;
	}

	DLL_EXPORT bool IsClosed(Transport* transport)
	{
		if (transport == nullptr)
			return true;

		return transport->IsClosed();
	}


	DLL_EXPORT const nlohmann::json* GetStats(Transport* transport)
	{
		nlohmann::json stat;
		
		if (transport == nullptr)
			return nullptr;
		
		try
		{
			stat = transport->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.GetStats]");
			stat = nlohmann::json::object();
		}

		return &stat;
	}

	//API comment : This method should be called when the server side transport has been closed (and vice-versa)
	DLL_EXPORT void Close(Transport* transport)
	{
		if (transport == nullptr)
			return;

		try
		{
			transport->Close();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.Close]");
		}
	}

	DLL_EXPORT void RestartIce(Transport* transport, const nlohmann::json* iceParameters)
	{
		if (transport == nullptr)
			return;

		try
		{
			transport->RestartIce(*iceParameters);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.RestartIce]");
		}
		
	}

	DLL_EXPORT void UpdateIceServers(Transport* transport, const nlohmann::json* iceServers)
	{
		if (transport == nullptr)
			return;
		try
		{
			transport->UpdateIceServers(*iceServers);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.UpdateIceServers]");
		}
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
		const nlohmann::json* appData = nullptr)
	{
		if (sendTransport == nullptr || producerListener == nullptr)
			return nullptr;

		
		if(appData == nullptr)
			sendTransport->Produce(producerListener, track, encodings, codecOptions, codec);
		else
			return sendTransport->Produce(producerListener, track, encodings, codecOptions, codec, *appData);
	}

	DLL_EXPORT DataProducer* ProduceData(
		SendTransport* sendTransport,
		DataProducer::Listener* listener,
		const char* label = "",
		const char* protocol = "",
		bool ordered = true,
		int maxRetransmits = 0,
		int maxPacketLifeTime = 0,
		const nlohmann::json* appData = nullptr)
	{
		DataProducer* dataProducer = nullptr;
		try
		{
			if(appData == nullptr)
				dataProducer = sendTransport->ProduceData(listener, label, protocol, ordered, maxRetransmits, maxPacketLifeTime);
			else
				dataProducer = sendTransport->ProduceData(listener, label, protocol, ordered, maxRetransmits, maxPacketLifeTime, *appData);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[SendTransport.ProduceData]");
			dataProducer = (DataProducer*)-1;
		}

		return dataProducer;
	}

	//SendTransport Listener
#pragma endregion

#pragma region RectTransport
	DLL_EXPORT Consumer* Consume(
		RecvTransport* recvTransport,
		Consumer::Listener* consumerListener,
		const char* id,
		const char* producerId,
		const char* kind,
		nlohmann::json* rtpParameters,
		const nlohmann::json* appData = nullptr)
	{
		if (recvTransport == nullptr || consumerListener == nullptr)
			return nullptr;
		Consumer* consumer = nullptr;
		try
		{
			if(appData == nullptr)
				consumer = recvTransport->Consume(consumerListener, id, producerId, kind, rtpParameters);
			else
				consumer = recvTransport->Consume(consumerListener, id, producerId, kind, rtpParameters, *appData);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[RecvTransport.Consume]");
			consumer = (Consumer*)-1;
		}

		return consumer;
	}

	//Check Pointer return 할당해서 돌려주는지 포인터에 복사하는지 체크할 필요 있음 소스코드 보고
	DLL_EXPORT DataConsumer* ConsumeData(
		RecvTransport* recvTransport,
		DataConsumer::Listener* listener,
		const char* id,
		const char* producerId,
		const uint16_t streamId,
		const char* label,
		const char* protocol = "",
		const nlohmann::json* appData = nullptr)
	{
		if (recvTransport == nullptr || listener == nullptr)
			return nullptr;
		DataConsumer* dataConsumer = nullptr;
		try
		{
			if(appData == nullptr)
				dataConsumer = recvTransport->ConsumeData(listener, id, producerId, streamId, label, protocol);
			else
				dataConsumer = recvTransport->ConsumeData(listener, id, producerId, streamId, label, protocol, *appData);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[RecvTransport.ConsumeData]");
			dataConsumer = (DataConsumer*)-1;
		}
		
		return dataConsumer;
	}
#pragma endregion

#pragma region Producer
	DLL_EXPORT const char* GetIdProducer(Producer* producer)
	{
		if (producer == nullptr)
			return "";
		string id;
		try
		{
			id = producer->GetId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetId]");
			id = "Error GetId";
		}
		const char* idPtr = id.c_str();
		
		return idPtr;
	}
	
	DLL_EXPORT const char* GetKind(Producer* producer)
	{
		if (producer == nullptr)
			return "";

		string kind;
		try
		{
			kind = producer->GetKind();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetKind]");
			kind = "Error Occurred";
		}
		
		const char* kindPtr = kind.c_str();

		return kindPtr;//Return audio or video
	}

	DLL_EXPORT webrtc::MediaStreamTrackInterface* GetTrack(Producer* producer)
	{
		if (producer == nullptr)
			return nullptr;
		webrtc::MediaStreamTrackInterface* trackInterface = nullptr;
		try
		{
			trackInterface = producer->GetTrack();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetTrack]");
			trackInterface = (webrtc::MediaStreamTrackInterface*)-1;
		}

		return trackInterface;
	}

	DLL_EXPORT const nlohmann::json* GetRtpParameters(Producer* producer)
	{
		if (producer == nullptr)
			return nullptr;

		nlohmann::json parameters;
		try
		{
			parameters = producer->GetRtpParameters();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetRtpParameters]");
		}

		return &parameters;
	}

	DLL_EXPORT const uint8_t GetMaxSpatialLayer(Producer* producer)
	{
		if (producer == nullptr)
			return 0;

		uint8_t layer = -1;
		try
		{
			layer = producer->GetMaxSpatialLayer();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetMaxSpatialLayer]");
		}

		return layer;
	}

	DLL_EXPORT nlohmann::json* GetStatsProducer(Producer* producer)
	{
		if (producer == nullptr)
			return nullptr;
		nlohmann::json stat;
		try
		{
			stat = producer->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetStats]");
		}

		return &stat;
	}

	DLL_EXPORT const nlohmann::json* GetAppData(Producer* producer)
	{
		if (producer == nullptr)
			return nullptr;
		nlohmann::json appData;
		try
		{
			appData = producer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetAppData]");
		}

		return &appData;
	}

	DLL_EXPORT bool IsClosedProducer(Producer* producer)
	{
		if (producer == nullptr)
			return true;
		return producer->IsClosed();
	}

	DLL_EXPORT bool IsPausedProducer(Producer* producer)
	{
		if (producer == nullptr)
			return false;
		return producer->IsPaused();
	}

	DLL_EXPORT void CloseProducer(Producer* producer)
	{
		if (producer == nullptr)
			return;

		try
		{
			producer->Close();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.Close]");
		}
	}

	DLL_EXPORT void PauseProducer(Producer* producer)
	{
		if (producer == nullptr)
			return;
		try
		{
			producer->Pause();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.Pause]");
		}
	}

	DLL_EXPORT void ResumeProducer(Producer* producer)
	{
		if (producer == nullptr)
			return;
		try
		{
			producer->Resume();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.Resume]");
		}
	}

	DLL_EXPORT void ReplaceTrack(Producer* producer, webrtc::MediaStreamTrackInterface* track)
	{
		if (producer == nullptr || track == nullptr)
			return;
		try
		{
			producer->ReplaceTrack(track);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.ReplaceTrack]");
		}
		
	}

	DLL_EXPORT void SetMaxSpatialLayer(Producer* producer, uint8_t spatialLayer)
	{
		if (producer == nullptr)
			return;

		try
		{
			producer->SetMaxSpatialLayer(spatialLayer);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.SetMaxSpatialTrack]");
		}
	}
#pragma endregion

#pragma region Consumer
	DLL_EXPORT const char* GetIdConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";

		string id;
		try
		{
			id = consumer->GetId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetId]");
		}
		const char* idPtr = id.c_str();
		
		return idPtr;
	}

	DLL_EXPORT const char* GetProducerIdConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";

		string producerId;
		try
		{
			producerId = consumer->GetProducerId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetProducerId]");
		}

		auto* producerIdPtr = producerId.c_str();
		
		return producerIdPtr;
	}

	DLL_EXPORT const char* GetKindConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return "";
		
		string kind;
		try
		{
			kind = consumer->GetKind();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetKind]");
		}

		const char* kindPtr = kind.c_str();
		return kindPtr;//Return audio or video
	}

	DLL_EXPORT webrtc::MediaStreamTrackInterface* GetTrackConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nullptr;
		
		webrtc::MediaStreamTrackInterface* track = nullptr;
		try
		{
			track = consumer->GetTrack();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetTrackConsumer]");
		}

		return track;
	}

	DLL_EXPORT const nlohmann::json* GetRtpParametersConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nullptr;

		nlohmann::json parameters;
		try
		{
			parameters = consumer->GetRtpParameters();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetRtpParameters]");
		}

		return &parameters;
	}

	DLL_EXPORT nlohmann::json* GetStatsConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nullptr;
		nlohmann::json stat;
		try
		{
			stat = consumer->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetStats]");
		}

		return &stat;
	}

	DLL_EXPORT const nlohmann::json* GetAppDataConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nullptr;
		nlohmann::json appData;
		try
		{
			appData = consumer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetAppDataConsumer]");
		}

		return &appData;
	}

	DLL_EXPORT bool IsClosedConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return true;
		return consumer->IsClosed();
	}

	DLL_EXPORT bool IsPausedConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return false;
		return consumer->IsPaused();
	}

	DLL_EXPORT void CloseConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		consumer->Close();
	}

	DLL_EXPORT void PauseConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		try
		{
			consumer->Pause();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.Pause]");
		}
	}

	DLL_EXPORT void ResumeConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return;
		try
		{
			consumer->Resume();
		}
		catch (exception e)
		{
			ErrorLogging(e,"[Consumer.Resume]");
		}
	}
#pragma endregion
	
#pragma region DataProducer
	DLL_EXPORT const char* GetIdDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return "";
		string id;
		try
		{
			id = dataProducer->GetId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetId]");
		}
		auto* idPtr = id.c_str();
		return idPtr;
	}

	DLL_EXPORT const nlohmann::json* GetSctpStreamParametersDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return nullptr;
		
		nlohmann::json result;
		try
		{
			result = dataProducer->GetSctpStreamParameters();
		}
		catch(exception e)
		{
			ErrorLogging(e, "[DataProducer.GetSctpStreamParameters]");
		}

		return &result;
	}

	DLL_EXPORT webrtc::DataChannelInterface::DataState GetReadyStateDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  webrtc::DataChannelInterface::DataState::kClosed;

		webrtc::DataChannelInterface::DataState state;
		try
		{
			state = dataProducer->GetReadyState();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetReadyState]");
		}

		return state;
	}

	DLL_EXPORT const char* GetLabelDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";

		string label;
		try
		{
			label = dataProducer->GetLabel();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetLabel]");
		}
		auto* labelPtr = label.c_str();
		return labelPtr;
	}

	DLL_EXPORT const char* GetProtocolDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";

		string protocol;

		try
		{
			protocol = dataProducer->GetProtocol();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetProtocol]");
		}

		auto* protocolPtr = protocol.c_str();
		return protocolPtr;
	}

	DLL_EXPORT const uint8_t GetBufferedAmountDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  0;
		uint8_t amount = 0;

		try
		{
			amount = dataProducer->GetBufferedAmount();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetBufferedAmount]");
		}
		
		return amount;
	}

	DLL_EXPORT const nlohmann::json* GetAppDataDataProducer(DataProducer* dataProducer)
	{
		auto result = nlohmann::json::object();
		if (dataProducer == nullptr)
			return  &result;
		
		try
		{
			result = dataProducer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.GetAppData]");
		}

		return &result;
	}

	DLL_EXPORT bool IsClosedDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return true;
		return dataProducer->IsClosed();
	}

	DLL_EXPORT void CloseDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return;
		try
		{
			dataProducer->Close();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.Close]");
		}
	}

	DLL_EXPORT void Send(DataProducer* dataProducer, webrtc::DataBuffer* buffer)
	{
		if (dataProducer == nullptr)
			return;
		try
		{
			dataProducer->Send(*buffer);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.Send]");
		}
	}
#pragma endregion

#pragma region DataConsumer
	DLL_EXPORT const char* GetIdDataConsumer(DataConsumer* dataConsumer)
	{
		string id;
		if (dataConsumer == nullptr)
			return "";

		try
		{
			id = dataConsumer->GetId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetId]");
		}

		auto* idPtr = id.c_str();
		return idPtr;
	}

	DLL_EXPORT const char* GetDataProducerIdDataConsumer(DataConsumer* dataConsumer)
	{
		string result;
		if (dataConsumer == nullptr)
			return "";

		try
		{
			result = dataConsumer->GetDataProducerId();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetDataProducerId]");
		}

		auto* resultPtr = result.c_str();
		return resultPtr;
	}

	DLL_EXPORT const nlohmann::json* GetSctpStreamParameters(DataConsumer* dataConsumer)
	{
		auto parameters = nlohmann::json::object();
		if (dataConsumer == nullptr)
			return &parameters;

		try
		{
			parameters = dataConsumer->GetSctpStreamParameters();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetSctpStreamParameters]");
		}

		return &parameters;
	}

	DLL_EXPORT webrtc::DataChannelInterface::DataState GetReadyStateDataConsumer(DataConsumer* dataConsumer)
	{
		auto state = webrtc::DataChannelInterface::DataState::kClosed;
		if (dataConsumer == nullptr)
			return  state;

		try
		{
			state = dataConsumer->GetReadyState();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetReadyState]");
		}

		return state;
	}

	DLL_EXPORT const char* GetLabel(DataConsumer* dataConsumer)
	{
		string label;
		if (dataConsumer == nullptr)
			return "";

		try
		{
			label = dataConsumer->GetLabel();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetLabel]");
		}
		
		auto* labelPtr = label.c_str();
		return labelPtr;
	}

	DLL_EXPORT const char* GetProtocol(DataConsumer* dataConsumer)
	{
		string protocol;
		if (dataConsumer == nullptr)
			return  "";
		
		try
		{
			protocol = dataConsumer->GetProtocol();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetProtocol]");
		}

		auto* protocolPtr = protocol.c_str();
		return protocolPtr;
	}

	DLL_EXPORT const nlohmann::json* GetAppDataDataConsumer(DataConsumer* dataConsumer)
	{
		auto appData = nlohmann::json::object();
		if (dataConsumer == nullptr)
			return  &appData;

		try
		{
			appData = dataConsumer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataConsumer.GetAppData]");
		}

		return &appData;
	}

	DLL_EXPORT bool IsClosedDataConsumer(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return true;
		return dataConsumer->IsClosed();
	}

	DLL_EXPORT void CloseDataConsumer(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return;

		try
		{
			dataConsumer->Close();
		}
		catch(exception e)
		{
			ErrorLogging(e, "[DataConsumer.Close]");
		}
	}

	DLL_EXPORT void SendDataProducer(DataProducer* dataProducer, webrtc::DataBuffer* buffer)
	{
		if (dataProducer == nullptr)
			return;

		try
		{
			dataProducer->Send(*buffer);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[DataProducer.Send]");
		}
	}
#pragma endregion

#pragma region Json Util
	DLL_EXPORT void GetJsonString(const nlohmann::json* jsonObject, char* text, size_t bufferSize)
	{
		if (jsonObject == nullptr)
			return;

		string jsonContainer;
		try
		{
			jsonContainer = jsonObject->dump();
			strcpy_s(text, bufferSize, jsonContainer.c_str());
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Json Util, GetJsonString]");
		}
	}
	
	DLL_EXPORT const char* TestId()
	{
		string* testId = new string("testDongho5309");
		return testId->c_str();
	}

	DLL_EXPORT uint8_t TestUint8(int testCase)
	{
		switch (testCase)
		{
		case -1:
		default:
			return testCase;
		}
	}

	DLL_EXPORT int TestEnumInput(webrtc::DataChannelInterface::DataState state)
	{
		try
		{
			switch (state)
			{
			case webrtc::DataChannelInterface::DataState::kConnecting:
				return 0;
			case webrtc::DataChannelInterface::DataState::kOpen:
				return 1;
			case webrtc::DataChannelInterface::DataState::kClosing:
				return 2;
			case webrtc::DataChannelInterface::DataState::kClosed:
				return 3;
			default:
				return -1;
			}
		}
		catch (exception e)
		{
			ErrorLogging(e, "[TestEnumInput]");
		}
	}

	DLL_EXPORT void __stdcall TestGetJsonString(char* text, size_t bufferSize)
	{
		const std::string strJson = R"(
    {
      "id" : 123456,
      "res" : 111.222,
      "name" : "Example Json",
      "desc" : "Hello, SnowDeer",
      "data" : [
        {
          "id" : 101,
          "name" : "snowdeer"
        },
        {
          "id" : 202,
          "name" : "ice-rabbit"
        }
      ],
      "info" : {
        "notebook" : "macbook m1 pro",
        "address" : "Seoul"
      }
    }
  )";

		auto jObj = nlohmann::json::parse(strJson);
		string jsonContainer;
		try
		{
			jsonContainer = jObj.dump();
			strcpy_s(text, bufferSize, jsonContainer.c_str());
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Json Util, GetJsonString]");
		}
	}
#pragma endregion
}

#pragma region Util
#include <fstream>
#include <ctime>
#include <iomanip>

// 현재시간을 string type으로 return하는 함수
const string currentDateTime()
{
	auto t = time(nullptr);
	tm timeManager;
	localtime_s(&timeManager, &t);
	ostringstream oss;
	oss << put_time(&timeManager, "%Y-%m-%d %H-%M");

	return oss.str();
}

void ErrorLogging(exception e, string prefix)
{
	string path = "ErrorLog_" + currentDateTime() + ".log";
	ofstream fileWriter(path.data(), ios_base::app);
	if (fileWriter.is_open())
	{
		fileWriter << prefix << e.what() << endl;
	}
	fileWriter.close();
}
#pragma endregion