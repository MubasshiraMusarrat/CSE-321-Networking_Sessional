#ifndef TCP_ADAPTIVE_RENO_H
#define TCP_ADAPTIVE_RENO_H

#include "tcp-congestion-ops.h"
#include "tcp-westwood-plus.h"

#include "ns3/data-rate.h"
#include "ns3/event-id.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/traced-value.h"

namespace ns3
{
    class Time;
    class TcpAdaptiveReno : public TcpWestwoodPlus
    {
        public:
        static TypeId GetTypeId();
        TcpAdaptiveReno();
        TcpAdaptiveReno(const TcpAdaptiveReno& sock);
        ~TcpAdaptiveReno() override;

        enum FilterType
        {
            NONE,
            TUSTIN
        };

        uint32_t GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight) override;

        void PktsAcked(Ptr<TcpSocketState> state, uint32_t packetsAcked, const Time& rtt) override;

        Ptr<TcpCongestionOps> Fork() override;

        private:
        void EstimateCongestionLevel(const Time& rtt);
        void EstimateIncWindow(Ptr<TcpSocketState> state);

        protected:
        void CongestionAvoidance (Ptr<TcpSocketState> state, uint32_t segmentsAcked);
        
        double m_congestionLevel;

        Time m_minRtt;
        Time m_packetLossRtt;
        Time m_conjecturedRtt;
        Time m_lastConjecturedRtt;

        int32_t m_incWindow;
        uint32_t m_baseWindow;
        int32_t m_probeWindow;
    };
} // namespace ns3

#endif /* TCP_ADAPTIVE_RENO_H */