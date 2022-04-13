using System;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;

namespace UtuakConnect
{
    internal class Program
    {
        static bool isServer;
        static TcpListener server;

        static void Main()
        {
            Console.WriteLine("s - сервер");
            isServer = Console.ReadKey().Key == ConsoleKey.S;
            if (isServer)
            {
                server = new TcpListener(IPAddress.Any, 8080);
                server.Start();
            }
            Start();
        }

        static void Start()
        {
            Console.Clear();
            Console.WriteLine("лобби - " + (isServer ? "сервер" : "клиент"));
            if (isServer)
            {
                TcpClient cli = server.AcceptTcpClient();
                NetworkStream nets = cli.GetStream();
                Console.Clear();
                Console.WriteLine("соединено");
                new Thread(() => GetMessage(cli, nets)).Start();
                while (true)
                {
                    string str = Console.ReadLine();
                    if (str != null)
                    {
                        nets.Write(Encoding.Unicode.GetBytes(str), 0, str.Length * 2);
                    }
                }
            }
            else
            {
                Console.WriteLine("ip-v4");
                TcpClient cli = new TcpClient(Console.ReadLine(), 8080);
                NetworkStream nets = cli.GetStream();
                Console.Clear();
                Console.WriteLine("соединено");
                new Thread(() => GetMessage(cli, nets)).Start();
                while (true)
                {
                    string str = Console.ReadLine();
                    if (str != null)
                    {
                        nets.Write(Encoding.Unicode.GetBytes(str), 0, str.Length * 2);
                    }
                }
            }
        }

        static void GetMessage(TcpClient cli, NetworkStream ns)
        {
            byte[] msg = new byte[256];
            while (true)
            {
                try
                {
                    int count = ns.Read(msg, 0, 256);
                    if (count > 0) Console.WriteLine(Encoding.Unicode.GetString(msg, 0, count));
                }
                catch
                {
                    Console.Clear();
                    break;
                }
                Thread.Sleep(250);
            }
            ns.Close();
            cli.Close();
            Start();
        }
    }
}
