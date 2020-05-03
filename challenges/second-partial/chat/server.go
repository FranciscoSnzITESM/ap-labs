// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"time"
	"strings"
)

//!+broadcaster
type clientCH chan<- string // an outgoing message channel
type client struct {
	channel clientCH
	name string
	ip string
	conn net.Conn
	isAdmin bool
}

var (
	clients  = make(map[clientCH]*client) // all connected clients
	entering = make(chan client)
	leaving  = make(chan clientCH)
	messages = make(chan string) // all incoming client messages
)

func broadcaster() {
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case clientInfo := <-entering:
			if len(clients) == 0 {
				clientInfo.isAdmin = true
			}
			clients[clientInfo.channel] = &clientInfo

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	var usernameTaken = false
	input := bufio.NewScanner(conn)
	input.Scan()
	who := input.Text()
	for _,p := range clients {
		if p.name == who {
			usernameTaken = true
			fmt.Fprintln(conn, "Sorry: Username already taken, please try again") // NOTE: ignoring network errors
			conn.Close()
			break;
		}
	}

	if !usernameTaken {
		ch := make(chan string) // outgoing client messages
		go clientWriter(conn, ch)

		ip := conn.RemoteAddr().String()
		ch <- serverTag + "Welcome to the Simple IRC Server"
		ch <- serverTag + "Your user [" + who + "] is successfully logged"
		fmt.Println(serverTag + "New connected user [" + who + "]")
		messages <- serverTag + "New connected user [" + who + "]"
		entering <- client{ch, who, ip, conn, false}
		// Check if admin when joined (first user)
		if clients[ch].isAdmin == true{
			ch <- serverTag + "Congrats, you were the first user."
			makeAdmin(who, ch)
		}

		for input.Scan() { // Incoming client messages
			msg := input.Text()
			if msg != "" {
				if msg[0] == '/' { // Command
					values := strings.Split(msg, " ")
					switch values[0] {
					case "/users":
						var userNames string
						for _,p := range clients{
							userNames += p.name + ", "
						}
						ch <- serverTag + userNames[:len(userNames)-2]
					case "/msg":
						if len(values) < 3 {
							ch <- serverTag + "Error in parameters, usage: /msg <user> <msg>"
						} else {
							var found = false
							for _,p := range clients {
								if p.name == values[1] {
									found = true
									p.channel <- who + " (direct) > " + msg[6+len(p.name):]
									break;
								}
							}
							if !found {
								ch <- serverTag + "Error, user not found"
							}
						}
					case "/time":
						// Ingoring potential errors from time.LoadLocation("Local")
						location,_ := time.LoadLocation("Local")
						loc := location.String()
						// If TZ not specified load with America/Mexico_City
						if loc == "Local"{
							curr,_ := time.LoadLocation("America/Mexico_City")
							loc = curr.String()
						}
						ch <- serverTag + "Local Time: " + loc + " " + time.Now().Format("15:04")
					case "/user":
						if len(values) != 2 {
							ch <- serverTag + "Error in parameters, usage: /user <user>"
						} else {
							var found = false
							for _,p := range clients {
								if p.name == values[1] {
									found = true
									ch <- serverTag + "username: " + p.name + ", IP: " + p.ip
									break;
								}
							}
							if !found {
								ch <- serverTag + "Error, user not found"
							}
						}
					case "/kick":
						if clients[ch].isAdmin {
							if len(values) != 2 {
								ch <- serverTag + "Error in parameters, usage: /kick <user>"
							} else {
								var found = false
								for _,p := range clients {
									if p.name == values[1] {
										found = true
										p.channel <- serverTag + "You're kicked from this channel"
										leaving <- p.channel
										fmt.Println(serverTag + p.name + " was kicked")
										messages <- serverTag + p.name + " was kicked from channel"
										p.conn.Close()
										break;
									}
								}
								if !found {
									ch <- serverTag + "Error, user not found"
								}
							}
						} else {
							ch <- serverTag + "Error, you need to be admin"
						}
					default:
						ch <- serverTag + "Error, unknown command"
					}
				} else { // Message
					messages <- who + " > " + msg
				}
			}
		}
		if clients[ch] != nil {
			// NOTE: ignoring potential errors from input.Err()
			leaving <- ch
			fmt.Println(serverTag + "[" + who + "] left")
			messages <- serverTag + "[" + who + "] left channel"
			conn.Close()
		}
	}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

func makeAdmin(who string, cli clientCH) {
	fmt.Println(serverTag + "["+ who + "] was promoted as the channel ADMIN")
	clients[cli].isAdmin = true
	cli <- serverTag + "You're the new IRC Server ADMIN"
}
//!-handleConn

//!+main

var serverTag string
func main() {
	if len(os.Args) != 5 {
		log.Fatal("Error in parameters, usage: go run client.go -host [host] -port [port]")
	}
	serverTag = "irc-server > "
	server := os.Args[2] + ":" + os.Args[4]
	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(serverTag + "Simple IRC Server started at " + server)
	go broadcaster()
	fmt.Println(serverTag + "Ready for receiving new clients")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
