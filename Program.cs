using System.Text;
using System.Net;
using System.Net.Sockets;

class Program
{
    private static void Main()
    {
        Console.WriteLine("Utuak games\nвсе права защищены\n2022");
        ConsoleKeyInfo cci = Console.ReadKey();
        if(cci.Key == ConsoleKey.S)
        {
            Console.Clear();
            Console.WriteLine("---");
            TcpListener server = new(IPAddress.Parse("127.0.0.1"), 8080);
            server.Start();
            TcpClient cli = server.AcceptTcpClient();
            NetworkStream nets = cli.GetStream();
            Console.Clear();
            Console.WriteLine("соединено");
            new Thread(() => GetMessage(cli, nets)).Start();
            while (cli.Connected)
            {
                string? str = Console.ReadLine();
                if (str != null)
                {
                     nets.Write(Encoding.Unicode.GetBytes(str));
                }
            }
        }
        else
        {
            Console.WriteLine("ip-v4");
            TcpClient cli = new(Console.ReadLine(), 8080);
            NetworkStream nets = cli.GetStream();
            Console.Clear();
            Console.WriteLine("соединено");
            new Thread(() => GetMessage(cli, nets)).Start();

            while (true)
            {
                string? str = Console.ReadLine();
                if (str != null)
                {
                     nets.Write(Encoding.Unicode.GetBytes(str));
                }
            }
        }
    }

    private static void GetMessage(TcpClient cli, NetworkStream ns)
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
        Main();
    }
}