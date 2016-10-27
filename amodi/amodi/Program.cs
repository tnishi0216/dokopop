using System;
using System.Collections.Generic;
//using System.Linq;
using System.Windows.Forms;
using System.Threading;
using System.Reflection;

namespace amodi
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
			Assembly asm = Assembly.GetEntryAssembly();
			string path = asm.Location;
			path = path.Replace('\\', '_');
			Mutex mutex = new Mutex(false, path);
			if (mutex.WaitOne(0, false) == false){
				// すでに起動している
				return;
			}

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new AutoMODIMainForm());

			mutex.ReleaseMutex();
        }
    }
}

