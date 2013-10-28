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

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <ns3/application.h>
#include <ns3/address.h>
#include <ns3/nstime.h>
#include <ns3/traced-callback.h>


namespace ns3 {


class Socket;
class Packet;
class HttpVariables;


class HttpServer : public Application
{
public:
  HttpServer ();
  virtual ~HttpServer ();
  static TypeId GetTypeId ();

  enum State_t
  {
    NOT_STARTED = 0,
    WAITING_CONNECTION_REQUEST,
    CONNECTED
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

  bool ConnectionRequestCallback (Ptr<Socket> socket, const Address & address);
  void NewConnectionCreatedCallback (Ptr<Socket> socket, const Address & address);
  void ReceivedDataCallback (Ptr<Socket> socket);

private:
  void ServeMainObject (Ptr<Socket> socket);
  void ServeEmbeddedObject (Ptr<Socket> socket);
  void SwitchToState (State_t state);

  State_t m_state;
  Ptr<Socket> m_initialSocket;
  std::list<Ptr<Socket> > m_acceptedSockets;
  uint32_t m_mtuSize;

  // ATTRIBUTES

  Ptr<HttpVariables> m_httpVariables;
  Address m_localAddress;
  uint16_t m_localPort;
  TypeId m_protocol;
  Time m_responseDelay;
  TracedCallback<Ptr<const Packet> > m_txTrace;
  TracedCallback<Ptr<const Packet>, const Address & > m_rxTrace;
  TracedCallback<std::string, std::string> m_stateTransitionTrace;

}; // end of `class HttpServer`


}  // end of `namespace ns3`


#endif /* HTTP_SERVER_H */