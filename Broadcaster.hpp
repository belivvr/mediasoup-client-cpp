#ifndef BROADCASTER_H
#define BROADCASTER_H
#define WEBRTC_WIN
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "mediasoupclient.hpp"
#include "json.hpp"
#include "DebugCpp.h"
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <string>

class Broadcaster : public
	mediasoupclient::SendTransport::Listener,
	mediasoupclient::RecvTransport::Listener,
	mediasoupclient::Producer::Listener,
	mediasoupclient::DataProducer::Listener,
	mediasoupclient::DataConsumer::Listener
{
public:
	struct TimerKiller
	{
		// returns false if killed:
		template<class R, class P>
		bool WaitFor(std::chrono::duration<R, P> const& time) const
		{
			std::unique_lock<std::mutex> lock(m);
			return !cv.wait_for(lock, time, [&] { return terminate; });
		}
		void Kill()
		{
			std::unique_lock<std::mutex> lock(m);
			terminate = true; // Should be modified inside mutex lock.
			cv.notify_all();  // It is safe, and *sometimes* optimal, to do this outside the lock.
		}

	private:
		mutable std::condition_variable cv;
		mutable std::mutex m;
		bool terminate = false;
	};

	/* Virtual methods inherited from SendTransport::Listener. */
public:
	std::future<void> OnConnect(
		mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters) override;
	void OnConnectionStateChange(
		mediasoupclient::Transport* transport, const std::string& connectionState) override;
	std::future<std::string> OnProduce(
		mediasoupclient::SendTransport* /*transport*/,
		const std::string& kind,
		nlohmann::json rtpParameters,
		const nlohmann::json& appData) override;

	std::future<std::string> OnProduceData(
		mediasoupclient::SendTransport* transport,
		const nlohmann::json& sctpStreamParameters,
		const std::string& label,
		const std::string& protocol,
		const nlohmann::json& appData) override;

	/* Virtual methods inherited from Producer::Listener. */
public:
	void OnTransportClose(mediasoupclient::Producer* producer) override;									// just log

	/* Virtual methods inherited from DataConsumer::Listener */
public:
	void OnMessage(mediasoupclient::DataConsumer* dataConsumer, const webrtc::DataBuffer& buffer) override; // just log On Message
	void OnConnecting(mediasoupclient::DataConsumer* dataConsumer) override
	{
		Debug::Log("[Broadcaster]OnConnecting");
	}
	void OnClosing(mediasoupclient::DataConsumer* dataConsumer) override								
	{
		Debug::Log("[Broadcaster]OnClosing");
	}
	void OnClose(mediasoupclient::DataConsumer* dataConsumer) override
	{
		Debug::Log("[Broadcaster]OnClose");
	}
	void OnOpen(mediasoupclient::DataConsumer* dataConsumer) override
	{
		Debug::Log("[Broadcaster]OnOpen");
	}
	void OnTransportClose(mediasoupclient::DataConsumer* dataConsumer) override
	{
		Debug::Log("[Broadcaster]OnTransportClose");
	}

	/* Virtual methods inherited from DataProducer::Listener */
public:
	void OnOpen(mediasoupclient::DataProducer* dataProducer) override;									// just log
	void OnClose(mediasoupclient::DataProducer* dataProducer) override;									// just log
	void OnBufferedAmountChange(mediasoupclient::DataProducer* dataProducer, uint64_t size) override;	// just log
	void OnTransportClose(mediasoupclient::DataProducer* dataProducer) override;						// just log

public:
	void Start(
		const std::string& baseUrl,
		bool enableAudio,
		bool useSimulcast,
		const nlohmann::json& routerRtpCapabilities,
		bool verifySsl = true);
	void Stop();

	~Broadcaster();


	mediasoupclient::Device device;
	mediasoupclient::SendTransport* sendTransport{ nullptr };
	mediasoupclient::RecvTransport* recvTransport{ nullptr };
	mediasoupclient::DataProducer* dataProducer{ nullptr };
	mediasoupclient::DataConsumer* dataConsumer{ nullptr };
private:
	std::string id = std::to_string(rtc::CreateRandomId());
	std::string baseUrl;
	std::thread sendDataThread;

	struct TimerKiller timerKiller;
	bool verifySsl = true;

	std::future<void> OnConnectSendTransport(const nlohmann::json& dtlsParameters);
	std::future<void> OnConnectRecvTransport(const nlohmann::json& dtlsParameters);

	void CreateSendTransport(bool enableAudio, bool useSimulcast);
	void CreateRecvTransport();
	void CreateDataConsumer();
};

#endif // STOKER_HPP
