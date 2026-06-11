using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using Tesseract;

namespace atsocr
{
    public partial class AutoTSOCRMainForm : Form
    {
        const string TESSDATA_DIR = "./tessdata";

        const int MARGIN_UNDER_CLICK = 2;	// 次の行までの空白

        const int WM_COPYDATA = 0x4A;
        const int WM_APP = 0x8000;
        const int WM_ATSOCR = (WM_APP + 0x400);
        const int WM_EXEC_OCR = (WM_APP + 0x410);

        // WM_COPYDATA command
        const int WMCD_EXISTCHECK = 0x4000;
		const int WMCD_SETPOINT = 0x4001;
		const int WMCD_RESTORE_WINDOW = 0x4002;


        //[StructLayout(LayoutKind.Sequential)]
        private struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int cbData;
            public IntPtr lpData;
        }

        public struct COPYDATASTRUCT_send {
            public Int32 dwData;        //送信する32ビット値
            public Int32 cbData;　　　　//lpDataのバイト数
            public string lpData;　　 //送信するデータへのポインタ(0も可能)
        }

        [DllImport("user32.dll", SetLastError = true)]
        private static extern int PostMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
        [DllImport("user32.dll", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, ref COPYDATASTRUCT_send lparam);
        [DllImport("User32.dll", EntryPoint = "FindWindow")]
        public static extern Int32 FindWindow(String lpClassName, String lpWindowName);

		// parser output //
		int CurLoc;	// mouse cursor position in text.

        public AutoTSOCRMainForm()
        {
			InitializeComponent();
            InitializeOcr();
        }
        void Cleanup()
        {
            CleanupOcr();
        }

        private void btnDoOCR_Click(object sender, EventArgs e)
        {
            DoOCR(tbFileName.Text);
        }

        System.IO.FileSystemWatcher fsw;
        Queue<string> FileNameQue;

        void InitializeOcr()
        {
            FileNameQue = new Queue<string>();
#if true
            string exepath = System.IO.Path.GetTempPath();
            exepath += "atsocr\\";

			try
			{
				System.IO.Directory.Delete(exepath);
			} catch { }
            System.IO.Directory.CreateDirectory(exepath);
#else
            string exepath = Application.StartupPath;
            lbFileName.Text = "Watching... - " + exepath;
#endif
            fsw = new System.IO.FileSystemWatcher();
            fsw.Path = exepath;
            fsw.IncludeSubdirectories = false;
            fsw.SynchronizingObject = this;
            fsw.Changed += new System.IO.FileSystemEventHandler(watcher_Changed);
            fsw.EnableRaisingEvents = true;
        }
        void CleanupOcr()
        {
            fsw.EnableRaisingEvents = false;
            fsw.Dispose();
            fsw = null;
        }

        private bool DoingOCR = false;
        string DoOCR(string filename)
        {
            if (DoingOCR) return "";
            DoingOCR = true;

            bool capture_page = miCapturePage.Checked;

            if (filename == ""){
                filename = SelectImageFromDirectory();
            }

            ParseFileName(filename);

            lbStatus.Text = "Recognizing... " + filename;
            tbText.Text = "";
            tbInfo.Text = "";
            TesseractEngine engine = null;
            Tesseract.Page page = null;
            for (int i = 0; i < 10; i++){
                try {
                    string language = OnlyAlbum ? "eng" : "eng+jpn";
                    engine = new TesseractEngine(TESSDATA_DIR, language);
                    using (var pix = Pix.LoadFromFile(filename))
                    {
                        DBW("DoOCR:OCR");
                        page = engine.Process(pix);
                        // 結果表示
                        DBW("DoOCR:Completed");
                        tbText.Text = page.GetText();
                    }
                } catch {
                    tbInfo.AppendText("ATSOCR Create failure (No tessdata?): " + filename + "\r\n");
                    DBW("ATSOCR Create failure (No tessdata?): " + filename);
                    System.Threading.Thread.Sleep(30);
                    continue;
                }
                break;
            }
            if (engine==null || page==null){
                return "";
            }
            const int UnderGap = MARGIN_UNDER_CLICK; // 単語のある領域より少し下のpointでも検索対象と認識する空白部分(Y方向)
            int last_x = 0;
            int lineno = 0;
			CurLoc = 0;
            lbPoint.Text = "" + CursorPoint.X + "," + CursorPoint.Y;
            String ocrText = "";
            int boxcount = 0;  // interatorしかないので取得できないらしい
            tbInfo.AppendText("Page:" + boxcount + " pt:" + CursorPoint.X + "," + CursorPoint.Y + "\r\n");
            using (var iter = page.GetIterator())
            {
                iter.Begin();

				bool outok = capture_page;
				string prevWord = "";
				string prevWord2 = "";

                bool cr = false;

                do
                {
                    string word = iter.GetText(PageIteratorLevel.Word);

                    if (string.IsNullOrWhiteSpace(word))
                        continue;

                    Rect rc;
                    if (iter.TryGetBoundingBox(PageIteratorLevel.Word, out rc))
                    {
                        bool curLocSet = false;
                        int h = rc.Y2 - rc.Y2;
                        int w = rc.X2 - rc.X1;
                        // DBW($"{word} ({rc.X1},{rc.Y1})-({rc.X2},{rc.Y2})");

                        //bool incursor;
						//tbText.AppendText(word+ " (" + rc.X1 + "," + rc.Y1+ ")\r\n");
                        if (CursorPoint.Y >= rc.Y1 && CursorPoint.Y <= rc.Y2+UnderGap){
                            //incursor = true;
							if (!outok)
							{
                                if (CursorPoint.X <= rc.X2){	// cursorが矩形内 or cursorを飛び越えた
                                    outok = true;
                                    DBW($"{word} ({rc.X1},{rc.Y1})-({rc.X2},{rc.Y2})");
									curLocSet = true;
	                                if (prevWord.Length != 0) {
										if (NumPrevWords >= 2 && prevWord2.Length != 0){
											ocrText += prevWord2 + " " + prevWord + " ";
                                            CurLoc = prevWord2.Length + 1 + prevWord.Length + 1;
										} else {
		                                    ocrText += prevWord + " ";
                                            CurLoc = prevWord.Length + 1;
                                        }
                                        // 正確なclick位置を反映（文字は等幅と仮定）
                                        // = (float)(CursorPoint.X - rc.X1) / (rc.X2 - rc.X1) * word.Length
                                        if (word.Length>0 && CursorPoint.X >= rc.X1) CurLoc += (CursorPoint.X - rc.X1) * word.Length / (rc.X2 - rc.X1);
	                                }
                                } else {
									prevWord2 = prevWord;
									prevWord = word;
								}
							}
                        } else {
                            //incursor = false;
                        }

                        if (outok){
                            tbInfo.AppendText(word+ " (" + w + "x" + h + ":" + rc.X1 + "," + rc.Y1 + ")\r\n");
                        }

                        if (last_x > rc.X1){
                            if (ocrText != "")
                                cr = true;
                            lineno++;
                        }
                        last_x = rc.X1;

                        if (outok){
                            if (cr){
                                ocrText += "\r\n";
                                if (curLocSet)
                                    CurLoc += 2;	// CR+LF
                            }

                            ocrText += word + " ";
                        }
                    }

                } while (iter.Next(PageIteratorLevel.Word));
            }
            lbStatus.Text = "Done. " + filename;
            DoingOCR = false;
            return ocrText;
        }
		private void ParseFileName(string filename)
		{
			Regex r = new Regex(@"\((?<x>\d+),(?<y>\d+)\)");
			Match m = r.Match(filename);
			if (m.Success)
			{
				CursorPoint.X = int.Parse(m.Groups["x"].Value);
				CursorPoint.Y = int.Parse(m.Groups["y"].Value);
			}
			r = new Regex(@"-n(?<n>\d+)");
			m = r.Match(filename);
			if (m.Success)
			{
				NumPrevWords = int.Parse(m.Groups["n"].Value);
			}
			r = new Regex(@"-a(?<a>\d+)");
			m = r.Match(filename);
			if (m.Success)
			{
                OnlyAlbum = (int.Parse(m.Groups["a"].Value) != 0);
			}
		}

        //イベントハンドラ
        private void watcher_Changed(System.Object source,
            System.IO.FileSystemEventArgs e)
        {
            switch (e.ChangeType)
            {
                case System.IO.WatcherChangeTypes.Changed:
                    string ext = System.IO.Path.GetExtension(e.FullPath);
                    if (ext==".gif" || ext==".jpg" || ext==".png" || ext==".tif" || ext==".bmp" || ext==".dib")
                    {
                        PostOCR(e.FullPath);
                    }
                    break;
//                case System.IO.WatcherChangeTypes.Created:
                    //break;
//                case System.IO.WatcherChangeTypes.Deleted:
  //                  break;
            }
        }

        private void PostOCR(string filename)
        {
            FileNameQue.Enqueue(filename);
            PostMessage(this.Handle, WM_EXEC_OCR, IntPtr.Zero, IntPtr.Zero);
        }

        private void ExecOCR(string filename)
        {
            string ocrText = DoOCR(filename);
            if (ocrText != "") {
                string textname = filename + ".txt";
                for (int i = 0; i < 10; i++) {
                    try {
                        StreamWriter writer = new StreamWriter(textname, false, System.Text.Encoding.GetEncoding("utf-16"));
                        writer.WriteLine(CurLoc.ToString());
                        writer.Write(ocrText);
                        writer.Close();
                    } catch {
                        tbInfo.AppendText("Write Error:" + textname + "\r\n");
                        System.Threading.Thread.Sleep(30);
                        continue;
                    }
                    DBW("ocrText="+ocrText);
                    break;
                }
            }
            if (!miDebugMode.Checked) {
                try {
                    System.IO.File.Delete(filename);
                }
                catch
                {
                    // 連続して.bmpファイルが作られているため
                    tbInfo.AppendText("Delete Error:" + filename + "\r\n");
                }
            }
        }

        private void miExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        enum ATSOCR_CMD
        {
            QUERY = 0,
            PAGE_CAPTURE = 1,
        };
        struct POINT
        {
            public int x;
            public int y;
        };
        Point CursorPoint;
        unsafe static Point int2point(IntPtr param)
        {
			POINT pt = (POINT)Marshal.PtrToStructure((IntPtr)param, typeof(POINT));
            Point p = new Point();
            p.X = pt.x;
            p.Y = pt.y;
            return p;
        }
        int NumPrevWords = 1;
        bool OnlyAlbum = true;
        // interface for external app
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_ATSOCR)
            {
                switch ((ATSOCR_CMD)m.WParam)
                {
                    case ATSOCR_CMD.QUERY:
                        m.Result = (IntPtr)m.LParam;
                        return;
                    case ATSOCR_CMD.PAGE_CAPTURE:
                        m.Result = (IntPtr)(miCapturePage.Checked ? 1 : 0);
                        return;
                    default:
                        break;
                }
            } else
            if (m.Msg == WM_EXEC_OCR) {
                string filename = "";
                while (FileNameQue.Count()!=0) {
                    filename = FileNameQue.Dequeue();
                }
                if (filename != "") {
                    ExecOCR(filename);
                }
            } else
            if (m.Msg == WM_COPYDATA)
            {
                COPYDATASTRUCT cds = new COPYDATASTRUCT();
                cds = (COPYDATASTRUCT)Marshal.PtrToStructure(m.LParam, typeof(COPYDATASTRUCT));
                if (cds.dwData == (IntPtr)WMCD_EXISTCHECK)
                {
                    m.Result = (IntPtr)1;
                    return;
                } else
				if (cds.dwData == (IntPtr)WMCD_SETPOINT){
					CursorPoint = int2point(cds.lpData);
					DBW("Cursor:" + CursorPoint.X + "," + CursorPoint.Y);
					return;
				} else
				if (cds.dwData == (IntPtr)WMCD_RESTORE_WINDOW)
				{
					this.Show();
				}
#if false   // 参考
                if (cds.cbData > 0)
                {
                    byte[] data = new byte[cds.cbData];
                    Marshal.Copy(cds.lpData, data, 0, cds.cbData);
                    //あとは data に COPYDATA の内容が入ってるので適宜処理
                }
                m.Result = (IntPtr)1;
#endif
            }
            base.WndProc(ref m);
        }

        private void miCapturePage_Click(object sender, EventArgs e)
        {
            miCapturePage.Checked = !miCapturePage.Checked;
        }

		private void AutoTSOCRMainForm_Activated(object sender, EventArgs e)
		{
			//this.Hide();
		}

		private void miDebugMode_Click(object sender, EventArgs e)
		{
			miDebugMode.Checked = !miDebugMode.Checked;
		}

		private void miUseDefLang_Click(object sender, EventArgs e)
		{
			miUseDefLang.Checked = !miUseDefLang.Checked;
		}

        private string SelectImageFromDirectory()
        {
            string directory = "c:\\temp\\atsocr";
            
            try
            {
                if (!System.IO.Directory.Exists(directory))
                {
                    MessageBox.Show("Directory not found: " + directory);
                    return "";
                }

                var imgFiles = System.IO.Directory.GetFiles(directory, "*.jpg");

                if (imgFiles.Length == 0){
                    imgFiles = System.IO.Directory.GetFiles(directory, "*.png");
                    if (imgFiles.Length == 0){
                        imgFiles = System.IO.Directory.GetFiles(directory, "*.bmp");
                        if (imgFiles.Length == 0){
                            MessageBox.Show("No JPG/PNG/BMP files found in " + directory);
                            return "";
                        }
                    }
                }

                // 最初に見つかったファイルを返す
                return imgFiles[0];
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }

            return "";
        }

        static int hWin = 0;
        void DBW( string msg )
        {
	        if (hWin == 0){
		        hWin = FindWindow( "TDbgMsgForm", "Debug Messenger" );
		        if ( hWin==0)
                {
                    hWin = -1;
                    return;
                }
	        } else if (hWin == -1) return;
            byte[] bytearray = System.Text.Encoding.Default.GetBytes(msg);
            int len = bytearray.Length;
	        COPYDATASTRUCT_send cds;
	        cds.dwData = 1;	// Indicate String
	        cds.cbData = len + 1;
            cds.lpData = msg;
	        SendMessage( (IntPtr)hWin, WM_COPYDATA, IntPtr.Zero, ref cds );
        }
    }
}
