/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <ns3/application.h>
#include <ns3/address.h>
#include <ns3/traced-callback.h>


namespace ns3 {


class Socket;
class Packet;
class HttpVariables;


class HttpClient : public Application
{
public:
  HttpClient ();
  virtual ~HttpClient ();
  static TypeId GetTypeId ();

  enum State_t
  {
    NOT_STARTED = 0,
    CONNECTING,
    EXPECTING_MAIN_OBJECT,
    PARSING_MAIN_OBJECT,
    EXPECTING_EMBEDDED_OBJECT,
    READING,
    STOPPED
  };

  State_t GetState () const;
  std::string GetStateString () const;
  static std::string GetStateString (State_t state);

protected:
  // Inherited from Object base class
  virtual void DoDispose ();

  // Inherited from Application base class
  virtual void StartApplication ();
  virtual void StopApplication ();

  // CALLBACK FUNCTIONS FROM SOCKET

  void ConnectionSucceededCallback (Ptr<Socket> socket);
  void ConnectionFailedCallback (Ptr<Socket> socket);
  void ReceivedDataCallback (Ptr<Socket> socket);

private:
  void RequestMainObject ();
  void RequestEmbeddedObject ();
  void ReceiveMainObject (Ptr<Socket> socket);
  void ReceiveEmbeddedObject (Ptr<Socket> socket);
  void EnterParsingTime ();
  void ParseMainObject ();
  void EnterReadingTime ();
  void SwitchToState (State_t state);

  State_t m_state;
  Ptr<Socket> m_socket;
  uint32_t m_embeddedObjectsToBeRequested;

  // ATTRIBUTES

  Ptr<HttpVariables> m_httpVariables;
  Address m_remoteServerAddress;
  uint16_t m_remoteServerPort;
  TypeId m_protocol;
  uint16_t m_requestSize;
  TracedCallback<Ptr<const Packet> > m_txMainObjectRequestTrace;
  TracedCallback<Ptr<const Packet> > m_txEmbeddedObjectRequestTrace;
  TracedCallback<Ptr<const Packet> > m_rxMainObjectPacketTrace;
  TracedCallback<Ptr<const Packet> > m_rxMainObjectTrace;
  TracedCallback<Ptr<const Packet> > m_rxEmbeddedObjectPacketTrace;
  TracedCallback<Ptr<const Packet> > m_rxEmbeddedObjectTrace;
  TracedCallback<std::string, std::string> m_stateTransitionTrace;

}; // end of `class HttpClient`


}  // end of `namespace ns3`


#endif /* HTTP_CLIENT_H */