#include "tcp-adaptive-reno.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include<algorithm>

NS_LOG_COMPONENT_DEFINE ("TcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpAdaptiveReno>()
    .AddAttribute("FilterType", "Use this to choose no filter or Tustin's approximation filter",
                  EnumValue(TcpAdaptiveReno::TUSTIN), MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                  MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
    .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                    MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                    "ns3::TracedValueCallback::DataRate")
  ;
  return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno ()
  : TcpWestwoodPlus (),
    m_congestionLevel (0),
    m_minRtt (Time (0)),
    m_packetLossRtt(Time (0)),
    m_conjecturedRtt(Time (0)),
    m_lastConjecturedRtt(Time(0)),
    m_incWindow (0),
    m_baseWindow(0),
    m_probeWindow (0)
{
  NS_LOG_FUNCTION (this);
}

TcpAdaptiveReno::TcpAdaptiveReno (const TcpAdaptiveReno& sock) :
  TcpWestwoodPlus(sock),
    m_congestionLevel (sock.m_congestionLevel),
    m_minRtt (sock.m_minRtt),
    m_packetLossRtt(sock.m_packetLossRtt),
    m_conjecturedRtt(sock.m_conjecturedRtt),
    m_lastConjecturedRtt(sock.m_lastConjecturedRtt),
    m_incWindow (sock.m_incWindow),
    m_baseWindow(sock.m_baseWindow),
    m_probeWindow (sock.m_probeWindow)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno (void)
{
}

void TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> state, uint32_t packetsAcked,const Time& rtt){
    NS_LOG_FUNCTION(this << state << packetsAcked << rtt);

    if (rtt.IsZero())
    {
        NS_LOG_WARN("RTT measured is zero!");
        return;
    }

    m_ackedSegments += packetsAcked;

    if(m_minRtt.IsZero() || rtt < m_minRtt){
        m_minRtt = rtt;
    }

    if (!(rtt.IsZero() || m_IsCount))
    {
        m_IsCount = true;
        EstimateBW(rtt, state);
    }
}

void TcpAdaptiveReno :: EstimateCongestionLevel(const Time& rtt){
    double a = m_lastConjecturedRtt < m_minRtt ? 0 : 0.85;
    m_conjecturedRtt =Seconds(a * m_lastConjecturedRtt.GetSeconds() + (1 - a) * m_packetLossRtt.GetSeconds());
    double temp = (rtt.GetSeconds() - m_minRtt.GetSeconds())/ (m_conjecturedRtt.GetSeconds()- m_minRtt.GetSeconds());
    m_congestionLevel = std :: min (temp,1.0);
}

void TcpAdaptiveReno :: EstimateIncWindow(Ptr<TcpSocketState> state){
    NS_LOG_FUNCTION(this << state);

    double M = 1000;
    double MSS = static_cast<double> (state->m_segmentSize * state->m_segmentSize);

    EstimateCongestionLevel(state->m_lastRtt);
    double temp = static_cast<double>(m_currentBW.Get().GetBitRate());
    //temp = temp / 8;
    //std::cout << m_currentBW <<"   "<<temp.GetBitRate() << std::endl;
    double m_maxIncWindow = temp/M * MSS;
    double alpha = 10;
    double beta = 2 * m_maxIncWindow * (1/alpha - (1/alpha + 1)/(std::exp(alpha)));
    double gamma = 1 - (2 * m_maxIncWindow * ((1/alpha) - ((1/alpha + 0.5)/(std::exp(alpha)))));

    m_incWindow = (int)((m_maxIncWindow / std::exp(alpha * m_congestionLevel)) + (beta * m_congestionLevel) + gamma);

    NS_LOG_LOGIC("Estimated Inc Window: " << m_incWindow);
}

void TcpAdaptiveReno :: CongestionAvoidance(Ptr<TcpSocketState> state, uint32_t segmentsAcked){
    NS_LOG_FUNCTION(this << state << segmentsAcked);

    if(segmentsAcked > 0){
        EstimateIncWindow(state);
        double MSSbyW = static_cast<double> ((state->m_segmentSize * state->m_segmentSize)/state->m_cWnd.Get());
        m_baseWindow += m_baseWindow + std :: max(MSSbyW,1.0);
        double temp = m_probeWindow + m_incWindow / state->m_cWnd.Get();
        m_probeWindow = std :: max(temp , 0.0);
        state->m_cWnd = m_baseWindow + m_probeWindow;
    }

    NS_LOG_LOGIC("Estimated cWnd: " << state->m_cWnd);
}

uint32_t TcpAdaptiveReno :: GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight){
    NS_LOG_FUNCTION(this << state << bytesInFlight);
    
    m_lastConjecturedRtt = m_conjecturedRtt;
    m_packetLossRtt = state->m_lastRtt;
    EstimateCongestionLevel(state->m_lastRtt);
    uint32_t temp = state->m_cWnd.Get() / (1 + m_congestionLevel);
    uint32_t ssthresh = std :: max (2*state->m_segmentSize,temp);
    m_probeWindow = 0;
    m_baseWindow = ssthresh;

    NS_LOG_LOGIC("Estimated ssthresh: " << ssthresh);
    return ssthresh;
}

Ptr<TcpCongestionOps> TcpAdaptiveReno ::Fork(){
    return CreateObject<TcpAdaptiveReno>(*this);
}

} // namespace ns3