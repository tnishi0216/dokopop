using Microsoft.Win32;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Media.Imaging;
using Windows.Graphics.Imaging;
using Windows.Media.Ocr;

namespace amsocr
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            InitializeOcr();
        }
        private async Task<OcrResult> RecognizeText(SoftwareBitmap snap)
        {
            OcrEngine ocrEngine = OcrEngine.TryCreateFromUserProfileLanguages();
            // OCR実行
            var ocrResult = await ocrEngine.RecognizeAsync(snap);
            return ocrResult;
        }
        private async Task<OcrResult> RunOcr(SoftwareBitmap sbitmap)
        {
            //OCRを実行する
            OcrEngine engine = OcrEngine.TryCreateFromLanguage(new Windows.Globalization.Language("ja-JP"));
            var result = await engine.RecognizeAsync(sbitmap);
            return result;
        }
        private async void btnPath_Click(object sender, RoutedEventArgs e)
        {
            ImgTarget.Source = null;
            txtPath.Text = "";

            await Task.Delay(10);

            //画像ファイルのパスを取得
            txtPath.Text = SelectPath();

            if (txtPath.Text != "")
            {
                //画像ファイルの読み込み
                ImgTarget.Source = System.Windows.Media.Imaging.BitmapFrame.Create(new Uri(txtPath.Text, UriKind.Absolute), BitmapCreateOptions.None, BitmapCacheOption.OnLoad);
            }
        }

        private string SelectPath()
        {
            var path = "";

            // ダイアログのインスタンスを生成
            var dialog = new OpenFileDialog();

            // ファイルの種類を設定
            dialog.Filter = "Image File(*.bmp, *.jpg, *.png, *.tif) | *.bmp; *.jpg; *.png; *.tif | Bitmap(*.bmp) | *.bmp | Jpeg(*.jpg) | *.jpg | PNG(*.png) | *.png";

            // ダイアログを表示する
            if (dialog.ShowDialog() == true)
            {
                // 選択されたファイル名を取得
                path = dialog.FileName;
            }

            return path;
        }

        private string SelectJpgFromDirectory()
        {
            string directory = "c:\\temp\\amsocr";
            
            try
            {
                if (!System.IO.Directory.Exists(directory))
                {
                    MessageBox.Show("Directory not found: " + directory);
                    return "";
                }

                var jpgFiles = System.IO.Directory.GetFiles(directory, "*.jpg");

                if (jpgFiles.Length == 0)
                {
                    MessageBox.Show("No JPG files found in " + directory);
                    return "";
                }

                // 最初に見つかったファイルを返す
                return jpgFiles[0];
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }

            return "";
        }

        private async void btnOcr_Click(object sender, RoutedEventArgs e)
        {
            //OCRの実行処理
            if (txtPath.Text != ""){
                var sbitmap = await ConvertSoftwareBitmap(ImgTarget);
                txtOcrResult.Text = (await RunOcr(sbitmap)).Text;
            } else {
                // ファイルを指定しない場合は、C:\temp\amsocr\*.jpgから任意のファイルを自動選択
                string imgpath = SelectJpgFromDirectory();
                if (imgpath != "")
                {
                    var ocrText = await DoOCR(imgpath);
                    txtOcrResult.AppendText(ocrText);
                }
            }
        }
        private async Task<SoftwareBitmap> ConvertSoftwareBitmap(System.Windows.Controls.Image image)
        {
            SoftwareBitmap sbitmap = null;

            using (MemoryStream stream = new MemoryStream())
            {
                //BmpBitmapEncoderに画像を書きこむ
                var encoder = new BmpBitmapEncoder();
                encoder.Frames.Add((System.Windows.Media.Imaging.BitmapFrame)image.Source);
                encoder.Save(stream);

                //メモリストリームを変換
                stream.Position = 0;  // ストリーム位置をリセット
                var irstream = WindowsRuntimeStreamExtensions.AsRandomAccessStream(stream);

                //画像データをSoftwareBitmapに変換
                var decorder = await Windows.Graphics.Imaging.BitmapDecoder.CreateAsync(irstream);
                sbitmap = await decorder.GetSoftwareBitmapAsync();
            }

            return sbitmap;
        }

        // ------------ amodi移植部分 --------------------- //
        const int MARGIN_UNDER_CLICK = 2;	// 次の行までの空白

        const int WM_COPYDATA = 0x4A;
        const int WM_APP = 0x8000;
        const int WM_AMSOCR = (WM_APP + 0x400);
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

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct COPYDATASTRUCT_send
        {
            public IntPtr dwData;        //送信する32ビット値
            public int cbData;           //lpDataのバイト数
            [MarshalAs(UnmanagedType.LPStr)]
            public string lpData;        //送信するデータへのポインタ(0も可能)
        }

        [DllImport("user32.dll", SetLastError = true)]
        private static extern int PostMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
        [DllImport("user32.dll", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, ref COPYDATASTRUCT_send lparam);
        [DllImport("User32.dll", EntryPoint = "FindWindow")]
        public static extern Int32 FindWindow(String lpClassName, String lpWindowName);

        System.IO.FileSystemWatcher fsw;
        Queue<string> FileNameQue;

        // parser output //
        int CurLoc;	// mouse cursor position in text.

        private bool InitializeOcr()
        {
            FileNameQue = new Queue<string>();

#if true
            string exepath = System.IO.Path.GetTempPath();
            exepath += "amsocr\\";

            try
            {
                System.IO.Directory.Delete(exepath);
            }
            catch { }
            System.IO.Directory.CreateDirectory(exepath);
#else
            string exepath = Application.StartupPath;
            lbFileName.Text = "Watching... - " + exepath;
#endif
            fsw = new System.IO.FileSystemWatcher();
            fsw.Path = exepath;
            fsw.IncludeSubdirectories = false;
            fsw.Changed += new System.IO.FileSystemEventHandler(watcher_Changed);
            fsw.EnableRaisingEvents = true;

            return true;
        }
        void CleanupOcr()
        {
            fsw.EnableRaisingEvents = false;
            fsw.Dispose();
            fsw = null;
        }
        private bool DoingOCR = false;
        private async Task<OcrResult> doOcrExec()
        {
            //OCRの実行処理
            var sbitmap = await ConvertSoftwareBitmap(ImgTarget);
            OcrEngine engine;
            if (miUseDefLang?.IsChecked ?? false)
            {
                string language = "ja-JP";
                engine = OcrEngine.TryCreateFromLanguage(new Windows.Globalization.Language(language));
            }
            else
            {
                //OCRを実行する
                engine = OcrEngine.TryCreateFromUserProfileLanguages();
            }
            return await engine.RecognizeAsync(sbitmap);
        }
        async Task<String> DoOCR(string filename)
        {
            if (DoingOCR) return "";
            DoingOCR = true;

            bool capture_page = (miCapturePage?.IsChecked ?? false);

            ParseFileName(filename);

            lbStatus.Content = "Recognizing... " + filename;
//            tbText.Text = "";
            tbInfo.Text = "";

            DBW("DoOCR:Read image file");
            ImgTarget.Source = System.Windows.Media.Imaging.BitmapFrame.Create(new Uri(filename, UriKind.Absolute), BitmapCreateOptions.None, BitmapCacheOption.OnLoad);

            OcrResult ocrResult;

            if (miDebugMode?.IsChecked ?? false)
            {
                foreach (var lang in OcrEngine.AvailableRecognizerLanguages)
                {
                    DBW($"{lang.LanguageTag}");
                }
            }

            DBW("DoOCR:OCR");
            try
            {
                ocrResult = await doOcrExec();
            }
            catch
            {
                DBW("OCR Error: " + filename);
                DBW("DoOCR:Closed");
                lbStatus.Content = "OCR Error: " + filename;
                DoingOCR = false;
                return "";
            }
            DBW("DoOCR:Completed");
            const int UnderGap = MARGIN_UNDER_CLICK; // 単語のある領域より少し下のpointでも検索対象と認識する空白部分(Y方向)
            int last_x = 0;
            int lineno = 0;
            CurLoc = 0;
            lbPoint.Content = "" + CursorPoint.X + "," + CursorPoint.Y;
            tbInfo.AppendText("Page:" + ocrResult.Lines.Count + " pt:" + CursorPoint.X + "," + CursorPoint.Y + "\r\n");
            String ocrText = "";
            foreach (var line in ocrResult.Lines)
            {
                bool outok = capture_page;
                string prevWord = "";
                string prevWord2 = "";
                foreach (var word in line.Words)
                {
                    bool cr = false;
                    bool curLocSet = false;

                    {
                        var rect = word.BoundingRect;
                        int h = (int)rect.Height;
                        int w = (int)rect.Width;

                        // DBW("Word:" + word.Text + " (" + w + "x" + h + ":" + rect.Left + "," + rect.Top + ")");

                        //bool incursor;
                        //tbText.AppendText(word.Text + " (" + rc.Left + "," + rc.Top+ ")\r\n");
                        if (CursorPoint.Y >= rect.Top && CursorPoint.Y <= rect.Bottom + UnderGap)
                        {
                            //incursor = true;
                            if (!outok)
                            {
                                if (CursorPoint.X < rect.Left // cursorを飛び越えた
                                    || (CursorPoint.X >= rect.Left && CursorPoint.X <= rect.Right)  // cursorが矩形内
                                    )
                                {
                                    outok = true;
                                    curLocSet = true;
                                    if (prevWord.Length != 0)
                                    {
                                        if (NumPrevWords >= 2 && prevWord2.Length != 0)
                                        {
                                            ocrText += prevWord2 + " " + prevWord + " ";
                                            CurLoc = prevWord2.Length + 1 + prevWord.Length + 1;
                                        }
                                        else
                                        {
                                            ocrText += prevWord + " ";
                                            CurLoc = prevWord.Length + 1;
                                        }
                                    }
                                }
                                else
                                {
                                    prevWord2 = prevWord;
                                    prevWord = word.Text;
                                }
                            }
                        }
                        else
                        {
                            //incursor = false;
                        }

                        if (outok)
                        {
                            tbInfo.AppendText(word.Text + " (" + w + "x" + h + ":" + rect.Left + "," + rect.Top + ")\r\n");
                        }

                        if (last_x > rect.Left)
                        {
                            if (ocrText != "")
                                cr = true;
                            lineno++;
                        }
                        last_x = (int)rect.Left;
                    }

                    if (outok)
                    {
                        if (cr)
                        {
                            ocrText += "\r\n";
                            if (curLocSet)
                                CurLoc += 2;	// CR+LF
                        }

                        ocrText += word.Text + " ";
                    }
                }
            }
            lbStatus.Content = "Done. " + filename;
            DoingOCR = false;
            return ocrText;
        }
        // (x,y)-n\d*.jpg x,y: click座標 n: clickn語前の単語(def.=1)
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
        }

        //イベントハンドラ
        private void watcher_Changed(object source, System.IO.FileSystemEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() => {
                string ext = System.IO.Path.GetExtension(e.FullPath);
                if (ext == ".gif" || ext == ".jpg" || ext == ".png" || ext == ".tif" || ext == ".bmp" || ext == ".dib")
                {
                    PostOCR(e.FullPath);
                }
            }));
        }

        private void PostOCR(string filename)
        {
            FileNameQue.Enqueue(filename);
            var hwnd = new System.Windows.Interop.WindowInteropHelper(this).Handle;
            PostMessage(hwnd, WM_EXEC_OCR, IntPtr.Zero, IntPtr.Zero);
        }

        private void ExecOCR(string filename)
        {
            var result = DoOCR(filename);
            if (result.Result != "")
            {
                string textname = filename + ".txt";
                for (int i = 0; i < 10; i++)
                {
                    try
                    {
                        StreamWriter writer = new StreamWriter(textname, false, System.Text.Encoding.GetEncoding("utf-16"));
                        writer.WriteLine(CurLoc.ToString());
                        writer.Write(result.Result);
                        writer.Close();
                    }
                    catch
                    {
                        tbInfo.AppendText("Write Error:" + textname + "\r\n");
                        System.Threading.Thread.Sleep(30);
                        continue;
                    }
                    break;
                }
            }
            if (!(miDebugMode?.IsChecked ?? false))
            {
                try
                {
                    System.IO.File.Delete(filename);
                }
                catch
                {
                    // 連続して.bmpファイルが作られているため
                    tbInfo.AppendText("Delete Error:" + filename + "\r\n");
                }
            }
        }

        enum AMSOCR_CMD
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
#if false
        unsafe static Point int2point(IntPtr param)
        {
            POINT pt = (POINT)Marshal.PtrToStructure((IntPtr)param, typeof(POINT));
            Point p = new Point();
            p.X = pt.x;
            p.Y = pt.y;
            return p;
        }
#endif
        int NumPrevWords = 1;

        //TODO: event handler

        static int hWin = 0;
        void DBW(string msg)
        {
            if (hWin == 0)
            {
                hWin = FindWindow("TDbgMsgForm", "Debug Messenger");
                if (hWin == 0) return;
            }
            byte[] bytearray = System.Text.Encoding.Default.GetBytes(msg);
            int len = bytearray.Length;
            COPYDATASTRUCT_send cds;
            cds.dwData = (IntPtr)1; // Indicate String
            cds.cbData = len + 1;
            cds.lpData = msg;
            SendMessage((IntPtr)hWin, WM_COPYDATA, IntPtr.Zero, ref cds);
        }


    }
}
