using System;
using System.IO;
using System.IO.Pipes;
using System.Diagnostics;
using System.Text;
using System.Threading;
using System.Windows.Forms;
namespace HV2Viewer
{
    static public class PipeServer
    {
		public delegate void StringRequestDelegate(string token);
		private static StringRequestDelegate StringRequest;
		
		private static Thread serverThread;
		public static void Start(StringRequestDelegate callback)
		{
			StringRequest = callback;
			serverThread = new Thread(ServerThread);
			serverThread.Start();
		}
		public static void Stop()
		{
			PipeClient.Write("*stop*");
		}
		private static void ServerThread(object data)
		{

			bool running = true;
			while (running)
			{
				NamedPipeServerStream pipeServer =
					new NamedPipeServerStream("__MSDN__HELP__PIPE__", PipeDirection.In, 1);
				try
				{
					pipeServer.WaitForConnection();

					StreamString ss = new StreamString(pipeServer);

					string token = ss.ReadString();

					if (token == "*stop*")
						running = false;
					else
						StringRequest(token);
				}
				catch(Exception)
				{
				}
				finally
				{
					pipeServer.Close();
				}
			}
		}
    }
    static public class PipeClient
    {
		public static void Write(string token)
		{
			
			NamedPipeClientStream pipeClient =
				new NamedPipeClientStream(".", "__MSDN__HELP__PIPE__",
					PipeDirection.Out);

			try
			{
				pipeClient.Connect(2000);

				StreamString ss = new StreamString(pipeClient);
				ss.WriteString(token);
				pipeClient.Close();
			}
			catch(Exception)
			{
			}
		} 		
    }
	public class StreamString
	{
		private Stream ioStream;
		private UnicodeEncoding streamEncoding;

		public StreamString(Stream ioStream)
		{
			this.ioStream = ioStream;
			streamEncoding = new UnicodeEncoding();
		}

		public string ReadString()
		{
			int len;
			len = ioStream.ReadByte() * 256;
			len += ioStream.ReadByte();
			byte[] inBuffer = new byte[len];
			ioStream.Read(inBuffer, 0, len);

			return streamEncoding.GetString(inBuffer);
		}

		public int WriteString(string outString)
		{
			byte[] outBuffer = streamEncoding.GetBytes(outString);
			int len = outBuffer.Length;
			if (len > UInt16.MaxValue)
			{
				len = (int)UInt16.MaxValue;
			}
			ioStream.WriteByte((byte)(len / 256));
			ioStream.WriteByte((byte)(len & 255));
			ioStream.Write(outBuffer, 0, len);
			ioStream.Flush();

			return outBuffer.Length + 2;
		}
	}
}
