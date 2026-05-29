using Microsoft.Win32;
using System.IO;
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

        private async void btnOcr_Click(object sender, RoutedEventArgs e)
        {
            //OCRの実行処理
            var sbitmap = await ConvertSoftwareBitmap(ImgTarget);
            txtOcrResult.Text = (await RunOcr(sbitmap)).Text;
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
                var irstream = WindowsRuntimeStreamExtensions.AsRandomAccessStream(stream);

                //画像データをSoftwareBitmapに変換
                var decorder = await Windows.Graphics.Imaging.BitmapDecoder.CreateAsync(irstream);
                sbitmap = await decorder.GetSoftwareBitmapAsync();
            }

            return sbitmap;
        }
    }
}
