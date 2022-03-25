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
		SendTransport* transport = nullptr;
		
		try
		{
			transport = device->CreateSendTransport(listener, iceParameters, iceCandidates, dtlsParameters, sctpParameters, peerConnectionOptions, appData);
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

		RecvTransport* transport = nullptr;
		try
		{
			transport = device->CreateRecvTransport(listener, id, iceParameters, iceCandidates, dtlsParameters, sctpParameters, peerConnectionOptions, appData);
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
	DLL_EXPORT const string GetId(Transport* transport)
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
		return id;
	}

	DLL_EXPORT const string GetConnectionState(Transport* transport)
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

		return state;
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


		nlohmann::json stat;
		try
		{
			stat = transport->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.GetStats]");
			stat = nlohmann::json::object();
		}

		return stat;
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

	DLL_EXPORT void RestartIce(Transport* transport, const nlohmann::json& iceParameters)
	{
		if (transport == nullptr)
			return;

		try
		{
			transport->RestartIce(iceParameters);
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Transport.RestartIce]");
		}
		
	}

	DLL_EXPORT void UpdateIceServers(Transport* transport, const nlohmann::json& iceServers = nlohmann::json())
	{
		if (transport == nullptr)
			return;
		try
		{
			transport->UpdateIceServers(iceServers);
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
		DataProducer* dataProducer = nullptr;
		try
		{
			dataProducer = sendTransport->ProduceData(listener, label, protocol, ordered, maxRetransmits, maxPacketLifeTime, appData);
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
		const std::string& id,
		const std::string& producerId,
		const std::string& kind,
		nlohmann::json* rtpParameters,
		const nlohmann::json& appData = nlohmann::json::object())
	{
		if (recvTransport == nullptr || consumerListener == nullptr)
			return nullptr;
		Consumer* consumer = nullptr;
		try
		{
			consumer = recvTransport->Consume(consumerListener, id, producerId, kind, rtpParameters, appData);
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
		DataConsumer* dataConsumer = nullptr;
		try
		{
			dataConsumer = recvTransport->ConsumeData(listener, id, producerId, streamId, label, protocol, appData);
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
	DLL_EXPORT const string& GetIdProducer(Producer* producer)
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

		return id;
	}
	
	DLL_EXPORT string GetKind(Producer* producer)
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
		
		return kind;//Return audio or video
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

	DLL_EXPORT const nlohmann::json& GetRtpParameters(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();

		nlohmann::json parameters;
		try
		{
			parameters = producer->GetRtpParameters();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetRtpParameters]");
		}

		return parameters;
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

	DLL_EXPORT nlohmann::json GetStatsProducer(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();
		nlohmann::json stat;
		try
		{
			stat = producer->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetStats]");
		}

		return stat;
	}

	DLL_EXPORT const nlohmann::json& GetAppData(Producer* producer)
	{
		if (producer == nullptr)
			return nlohmann::json::object();
		nlohmann::json appData;
		try
		{
			appData = producer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Producer.GetAppData]");
		}

		return appData;
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
	DLL_EXPORT const string& GetIdConsumer(Consumer* consumer)
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

		return id;
	}

	DLL_EXPORT const string& GetProducerIdConsumer(Consumer* consumer)
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

		return producerId;
	}

	DLL_EXPORT string GetKindConsumer(Consumer* consumer)
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

		return kind;//Return audio or video
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

	DLL_EXPORT const nlohmann::json& GetRtpParametersConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();

		nlohmann::json parameters;
		try
		{
			parameters = consumer->GetRtpParameters();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetRtpParameters]");
		}

		return parameters;
	}

	DLL_EXPORT nlohmann::json GetStatsConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();
		nlohmann::json stat;
		try
		{
			stat = consumer->GetStats();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetStats]");
		}

		return stat;
	}

	DLL_EXPORT const nlohmann::json& GetAppDataConsumer(Consumer* consumer)
	{
		if (consumer == nullptr)
			return nlohmann::json::object();
		nlohmann::json appData;
		try
		{
			appData = consumer->GetAppData();
		}
		catch (exception e)
		{
			ErrorLogging(e, "[Consumer.GetAppDataConsumer]");
		}

		return appData;
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
	DLL_EXPORT const string& GetIdDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return "";
		return dataProducer->GetId();
	}

	DLL_EXPORT const nlohmann::json& GetSctpStreamParametersDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return nlohmann::json::object();
		return dataProducer->GetSctpStreamParameters();
	}

	DLL_EXPORT webrtc::DataChannelInterface::DataState GetReadyStateDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  webrtc::DataChannelInterface::DataState::kClosed;
		return dataProducer->GetReadyState();
	}

	DLL_EXPORT const string& GetLabelDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";
		return dataProducer->GetLabel();
	}

	DLL_EXPORT const string& GetProtocolDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  "";
		return dataProducer->GetProtocol();
	}

	DLL_EXPORT const uint8_t GetBufferedAmountDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  -1;
		return dataProducer->GetBufferedAmount();
	}

	DLL_EXPORT const nlohmann::json& GetAppDataDataProducer(DataProducer* dataProducer)
	{
		if (dataProducer == nullptr)
			return  nlohmann::json::object();
		return dataProducer->GetAppData();
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
	DLL_EXPORT const string& GetIdDataConsumer(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return "";
		return dataConsumer->GetId();
	}

	DLL_EXPORT const string& GetDataProducerIdDataConsumer(DataConsumer* dataConsumer)
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

	DLL_EXPORT const nlohmann::json& GetAppDataDataConsumer(DataConsumer* dataConsumer)
	{
		if (dataConsumer == nullptr)
			return  nlohmann::json::object();
		return dataConsumer->GetAppData();
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
		dataConsumer->Close();
	}

	DLL_EXPORT void SendDataProducer(DataProducer* dataProducer, webrtc::DataBuffer& buffer)
	{
		if (dataProducer == nullptr)
			return;
		dataProducer->Send(buffer);
	}
#pragma endregion

#pragma region Logging

#pragma endregion
}


#include <fstream>
#include <ctime>
#include <iomanip>

#pragma region Util
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

void ErrorLogging(exception e, string prefix = "")
{
	string path = "ErrorLog_" + currentDateTime() + ".log";
	ofstream fileWriter(path.data(), ios_base::app);
	if (fileWriter.is_open())
	{
		fileWriter << "[mediasoupclient_library]" << prefix << e.what() << endl;
	}
	fileWriter.close();
}
#pragma endregion