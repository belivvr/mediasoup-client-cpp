#include "Broadcaster.hpp"
#include "MediaStreamTrackFactory.hpp"
#include "mediasoupclient.hpp"
#include "json.hpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

using json = nlohmann::json;

Broadcaster::~Broadcaster()
{
	this->Stop();
}

void Broadcaster::OnTransportClose(mediasoupclient::Producer* /*producer*/)
{
	Debug::Log("[INFO] Broadcaster::OnTransportClose()");
}

void Broadcaster::OnTransportClose(mediasoupclient::DataProducer* /*dataProducer*/)
{
	Debug::Log("[INFO] Broadcaster::OnTransportClose()");
}

/* Transport::Listener::OnConnect
 *
 * Fired for the first Transport::Consume() or Transport::Produce().
 * Update the already created remote transport with the local DTLS parameters.
 */
std::future<void> Broadcaster::OnConnect(mediasoupclient::Transport* transport, const json& dtlsParameters)
{
	Debug::Log("[INFO] Broadcaster::OnConnect()");
	Debug::Log("[INFO] dtlsParameters: " + dtlsParameters.dump(4));

	if (transport->GetId() == this->sendTransport->GetId())
	{
		return this->OnConnectSendTransport(dtlsParameters);
	}
	else if (transport->GetId() == this->recvTransport->GetId())
	{
		return this->OnConnectRecvTransport(dtlsParameters);
	}
	else
	{
		std::promise<void> promise;

		promise.set_exception(std::make_exception_ptr("Unknown transport requested to connect"));

		return promise.get_future();
	}
}

std::future<void> Broadcaster::OnConnectSendTransport(const json& dtlsParameters)
{
	std::promise<void> promise;

	/* clang-format off */
	json body =
	{
		{ "dtlsParameters", dtlsParameters }
	};
	/* clang-format on */

	//여기서 connect에 dtls Param을 넘긴다.
	promise.set_value();
	//실패시 exception 설정
	promise.set_exception(std::make_exception_ptr("error"));
	return promise.get_future();
}

std::future<void> Broadcaster::OnConnectRecvTransport(const json& dtlsParameters)
{
	std::promise<void> promise;

	/* clang-format off */
	json body =
	{
		{ "dtlsParameters", dtlsParameters }
	};
	/* clang-format on */

	/*auto r = cpr::PostAsync(
		cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports/" +
				  this->recvTransport->GetId() + "/connect" },
		cpr::Body{ body.dump() },
		cpr::Header{ { "Content-Type", "application/json" } },
		cpr::VerifySsl{ verifySsl })
		.get();*/

	//여기서 connect에 dtls Param을 넘긴다.
	promise.set_value();
	//실패시 exception 설정
	promise.set_exception(std::make_exception_ptr("error"));
	return promise.get_future();
}

/*
 * Transport::Listener::OnConnectionStateChange.
 */
void Broadcaster::OnConnectionStateChange(
	mediasoupclient::Transport* /*transport*/, const std::string& connectionState)
{
	std::string log("[INFO] Broadcaster::OnConnectionStateChange() [connectionState:" + connectionState + "]");
	Debug::Log(log);

	if (connectionState == "failed")
	{
		Debug::Log("connectionState Failed!", Color::Red);
		Stop();
	}
}

/* Producer::Listener::OnProduce
 *
 * Fired when a producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
std::future<std::string> Broadcaster::OnProduce(
	mediasoupclient::SendTransport* /*transport*/,
	const std::string& kind,
	json rtpParameters,
	const json& /*appData*/)
{
	Debug::Log("[INFO] Broadcaster::OnProduce()");
	Debug::Log("[INFO] rtpParameters: " + rtpParameters.dump(4));

	std::promise<std::string> promise;

	/* clang-format off */
	json body =
	{
		{ "kind",          kind          },
		{ "rtpParameters", rtpParameters }
	};
	/* clang-format on */

	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports/" +
	//			  this->sendTransport->GetId() + "/producers" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();

	// 성공시
	auto response = json::parse("response");

	auto it = response.find("id");
	if (it == response.end() || !it->is_string())
	{
		promise.set_exception(std::make_exception_ptr("'id' missing in response"));
	}

	promise.set_value((*it).get<std::string>());
	//실패시
	Debug::Log("[ERROR] unable to create producer");

	promise.set_exception(std::make_exception_ptr("error"));

	return promise.get_future();
}

/* Producer::Listener::OnProduceData
 *
 * Fired when a data producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
std::future<std::string> Broadcaster::OnProduceData(
	mediasoupclient::SendTransport* /*transport*/,
	const json& sctpStreamParameters,
	const std::string& label,
	const std::string& protocol,
	const json& /*appData*/)
{
	Debug::Log( "[INFO] Broadcaster::OnProduceData()");
	// Debug::Log( "[INFO] rtpParameters: " << rtpParameters.dump(4));

	std::promise<std::string> promise;

	/* clang-format off */
	json body =
	{
		{ "label"                , label },
		{ "protocol"             , protocol },
		{ "sctpStreamParameters" , sctpStreamParameters }
		// { "appData"				 , "someAppData" }
	};
	/* clang-format on */

	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports/" +
	//			  this->sendTransport->GetId() + "/produce/data" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();

	// 성공시
	auto response = json::parse("response");

	auto it = response.find("id");
	if (it == response.end() || !it->is_string())
	{
		promise.set_exception(std::make_exception_ptr("'id' missing in response"));
	}

	promise.set_value((*it).get<std::string>());
	//실패시
	Debug::Log("[ERROR] unable to create producer");

	promise.set_exception(std::make_exception_ptr("error"));

	return promise.get_future();
}

void Broadcaster::Start(
	const std::string& baseUrl,
	bool enableAudio,
	bool useSimulcast,
	const json& routerRtpCapabilities,
	bool verifySsl)
{
	Debug::Log("[INFO] Broadcaster::Start()");

	this->baseUrl = baseUrl;
	this->verifySsl = verifySsl;

	// Load the device.
	this->device.Load(routerRtpCapabilities);

	Debug::Log("[INFO] creating Broadcaster...");

	/* clang-format off */
	json body =
	{
		{ "id",          this->id          },
		{ "displayName", "broadcaster"     },
		{ "device",
			{
				{ "name",    "libmediasoupclient"       },
				{ "version", mediasoupclient::Version() }
			}
		},
		{ "rtpCapabilities", this->device.GetRtpCapabilities() }
	};
	/* clang-format on */

	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();

	//if (r.status_code != 200)
	//{
	//	Debug::Log("[ERROR] unable to create Broadcaster"
	//		<< " [status code:" << r.status_code << ", body:\"" << r.text << "\"]");

	//	return;
	//}

	this->CreateSendTransport(enableAudio, useSimulcast);
	this->CreateRecvTransport();
}

void Broadcaster::CreateDataConsumer()
{
	const std::string& dataProducerId = this->dataProducer->GetId();

	/* clang-format off */
	json body =
	{
		{ "dataProducerId", dataProducerId }
	};
	/* clang-format on */
	//create server data consumer
	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports/" +
	//			  this->recvTransport->GetId() + "/consume/data" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();
	if (r.status_code != 200)
	{
		Debug::Log("[ERROR] server unable to consume mediasoup recv WebRtcTransport [status code:" << r.status_code << ", body:\"" << r.text << "\"]");
		return;
	}

	auto response = json::parse(r.text);
	if (response.find("id") == response.end())
	{
		Debug::Log("[ERROR] 'id' missing in response");
		return;
	}
	auto dataConsumerId = response["id"].get<std::string>();

	if (response.find("streamId") == response.end())
	{
		Debug::Log("[ERROR] 'streamId' missing in response");
		return;
	}
	auto streamId = response["streamId"].get<uint16_t>();

	// Create client consumer.
	this->dataConsumer = this->recvTransport->ConsumeData(
		this, dataConsumerId, dataProducerId, streamId, "chat", "", nlohmann::json());
}

void Broadcaster::CreateSendTransport(bool enableAudio, bool useSimulcast)
{
	Debug::Log("[INFO] creating mediasoup send WebRtcTransport...");

	json sctpCapabilities = this->device.GetSctpCapabilities();
	/* clang-format off */
	json body =
	{
		{ "type",    "webrtc" },
		{ "rtcpMux", true     },
		{ "sctpCapabilities", sctpCapabilities }
	};
	/* clang-format on */

	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();

	//if (r.status_code != 200)
	//{
	//	Debug::Log("[ERROR] unable to create send mediasoup WebRtcTransport"
	//		<< " [status code:" << r.status_code << ", body:\"" << r.text << "\"]");

	//	return;
	//}

	//auto response = json::parse(r.text);

	if (response.find("id") == response.end())
	{
		Debug::Log("[ERROR] 'id' missing in response");

		return;
	}
	else if (response.find("iceParameters") == response.end())
	{
		Debug::Log("[ERROR] 'iceParametersd' missing in response");

		return;
	}
	else if (response.find("iceCandidates") == response.end())
	{
		Debug::Log("[ERROR] 'iceCandidates' missing in response");

		return;
	}
	else if (response.find("dtlsParameters") == response.end())
	{
		Debug::Log("[ERROR] 'dtlsParameters' missing in response");

		return;
	}
	else if (response.find("sctpParameters") == response.end())
	{
		Debug::Log("[ERROR] 'sctpParameters' missing in response");

		return;
	}

	Debug::Log( "[INFO] creating SendTransport...");

	auto sendTransportId = response["id"].get<std::string>();

	this->sendTransport = this->device.CreateSendTransport(
		this,
		sendTransportId,
		response["iceParameters"],
		response["iceCandidates"],
		response["dtlsParameters"],
		response["sctpParameters"]);

	///////////////////////// Create Audio Producer //////////////////////////

	if (enableAudio && this->device.CanProduce("audio"))
	{
		auto audioTrack = createAudioTrack(std::to_string(rtc::CreateRandomId()));

		/* clang-format off */
		json codecOptions = {
			{ "opusStereo", true },
			{ "opusDtx",		true }
		};
		/* clang-format on */

		this->sendTransport->Produce(this, audioTrack, nullptr, &codecOptions, nullptr);
	}
	else
	{
		Debug::Log("[WARN] cannot produce audio" << std::endl;
	}

	///////////////////////// Create Video Producer //////////////////////////

	if (this->device.CanProduce("video"))
	{
		auto videoTrack = createSquaresVideoTrack(std::to_string(rtc::CreateRandomId()));

		if (useSimulcast)
		{
			std::vector<webrtc::RtpEncodingParameters> encodings;
			encodings.emplace_back(webrtc::RtpEncodingParameters());
			encodings.emplace_back(webrtc::RtpEncodingParameters());
			encodings.emplace_back(webrtc::RtpEncodingParameters());

			this->sendTransport->Produce(this, videoTrack, &encodings, nullptr, nullptr);
		}
		else
		{
			this->sendTransport->Produce(this, videoTrack, nullptr, nullptr, nullptr);
		}
	}
	else
	{
		Debug::Log("[WARN] cannot produce video");

		return;
	}

	///////////////////////// Create Data Producer //////////////////////////

	this->dataProducer = sendTransport->ProduceData(this);

	uint32_t intervalSeconds = 10;
	std::thread([this, intervalSeconds]() {
		bool run = true;
		while (run)
		{
			std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
			std::time_t t = std::chrono::system_clock::to_time_t(p);
			std::string s = std::ctime(&t);
			auto dataBuffer = webrtc::DataBuffer(s);
			Debug::Log( "[INFO] sending chat data: " + s);
			this->dataProducer->Send(dataBuffer);
			run = timerKiller.WaitFor(std::chrono::seconds(intervalSeconds));
		}
		})
		.detach();
}

void Broadcaster::CreateRecvTransport()
{
	Debug::Log( "[INFO] creating mediasoup recv WebRtcTransport...");

	json sctpCapabilities = this->device.GetSctpCapabilities();
	/* clang-format off */
	json body =
	{
		{ "type",    "webrtc" },
		{ "rtcpMux", true     },
		{ "sctpCapabilities", sctpCapabilities }
	};
	/* clang-format on */

	// create server transport
	//auto r = cpr::PostAsync(
	//	cpr::Url{ this->baseUrl + "/broadcasters/" + this->id + "/transports" },
	//	cpr::Body{ body.dump() },
	//	cpr::Header{ { "Content-Type", "application/json" } },
	//	cpr::VerifySsl{ verifySsl })
	//	.get();

	if (r.status_code != 200)
	{
		Debug::Log("[ERROR] unable to create mediasoup recv WebRtcTransport [status code:" + r.status_code + ", body:\"" + r.text + "\"]", Color::Red);

		return;
	}

	auto response = json::parse(r.text);

	if (response.find("id") == response.end())
	{
		Debug::Log("[ERROR] 'id' missing in response", Color::Red);

		return;
	}
	else if (response.find("iceParameters") == response.end())
	{
		Debug::Log("[ERROR] 'iceParameters' missing in response", Color::Red);

		return;
	}
	else if (response.find("iceCandidates") == response.end())
	{
		Debug::Log("[ERROR] 'iceCandidates' missing in response", Color::Red);

		return;
	}
	else if (response.find("dtlsParameters") == response.end())
	{
		Debug::Log("[ERROR] 'dtlsParameters' missing in response", Color::Red);

		return;
	}
	else if (response.find("sctpParameters") == response.end())
	{
		Debug::Log("[ERROR] 'sctpParameters' missing in response", Color::Red);

		return;
	}

	auto recvTransportId = response["id"].get<std::string>();

	Debug::Log("[INFO] creating RecvTransport...");

	auto sctpParameters = response["sctpParameters"];

	this->recvTransport = this->device.CreateRecvTransport(
		this,
		recvTransportId,
		response["iceParameters"],
		response["iceCandidates"],
		response["dtlsParameters"],
		sctpParameters);

	this->CreateDataConsumer();
}

void Broadcaster::OnMessage(mediasoupclient::DataConsumer* dataConsumer, const webrtc::DataBuffer& buffer)
{
	Debug::Log( "[INFO] Broadcaster::OnMessage()");
	if (dataConsumer->GetLabel() == "chat")
	{
		std::string s = std::string(buffer.data.data<char>(), buffer.data.size());
		Debug::Log( "[INFO] received chat data: " + s);
	}
}

void Broadcaster::Stop()
{
	Debug::Log("[INFO] Broadcaster::Stop()");

	this->timerKiller.Kill();

	if (this->recvTransport)
	{
		recvTransport->Close();
	}

	if (this->sendTransport)
	{
		sendTransport->Close();
	}
}

void Broadcaster::OnOpen(mediasoupclient::DataProducer* /*dataProducer*/)
{
	Debug::Log("[INFO] Broadcaster::OnOpen()");
}
void Broadcaster::OnClose(mediasoupclient::DataProducer* /*dataProducer*/)
{
	Debug::Log("[INFO] Broadcaster::OnClose()");
}
void Broadcaster::OnBufferedAmountChange(mediasoupclient::DataProducer* /*dataProducer*/, uint64_t /*size*/)
{
	Debug::Log("[INFO] Broadcaster::OnBufferedAmountChange()");
}
