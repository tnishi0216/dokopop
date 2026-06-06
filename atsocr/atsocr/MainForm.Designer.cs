namespace atsocr
{
    partial class AutoTSOCRMainForm
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tbInfo = new System.Windows.Forms.TextBox();
            this.lbStatus = new System.Windows.Forms.Label();
            this.tbText = new System.Windows.Forms.TextBox();
            this.btnDoOCR = new System.Windows.Forms.Button();
            this.tbFileName = new System.Windows.Forms.TextBox();
            this.lbFileName = new System.Windows.Forms.Label();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.miCapturePage = new System.Windows.Forms.ToolStripMenuItem();
            this.miUseDefLang = new System.Windows.Forms.ToolStripMenuItem();
            this.miDebugMode = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.miExit = new System.Windows.Forms.ToolStripMenuItem();
            this.lbPoint = new System.Windows.Forms.Label();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tbInfo
            // 
            this.tbInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbInfo.Location = new System.Drawing.Point(18, 321);
            this.tbInfo.Multiline = true;
            this.tbInfo.Name = "tbInfo";
            this.tbInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbInfo.Size = new System.Drawing.Size(262, 51);
            this.tbInfo.TabIndex = 16;
            // 
            // lbStatus
            // 
            this.lbStatus.AutoSize = true;
            this.lbStatus.Location = new System.Drawing.Point(24, 93);
            this.lbStatus.Name = "lbStatus";
            this.lbStatus.Size = new System.Drawing.Size(38, 12);
            this.lbStatus.TabIndex = 15;
            this.lbStatus.Text = "Status";
            // 
            // tbText
            // 
            this.tbText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbText.Location = new System.Drawing.Point(17, 116);
            this.tbText.Multiline = true;
            this.tbText.Name = "tbText";
            this.tbText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbText.Size = new System.Drawing.Size(265, 199);
            this.tbText.TabIndex = 14;
            // 
            // btnDoOCR
            // 
            this.btnDoOCR.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnDoOCR.Location = new System.Drawing.Point(89, 53);
            this.btnDoOCR.Name = "btnDoOCR";
            this.btnDoOCR.Size = new System.Drawing.Size(98, 28);
            this.btnDoOCR.TabIndex = 13;
            this.btnDoOCR.Text = "Do OCR";
            this.btnDoOCR.UseVisualStyleBackColor = true;
            this.btnDoOCR.Click += new System.EventHandler(this.btnDoOCR_Click);
            // 
            // tbFileName
            // 
            this.tbFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbFileName.Location = new System.Drawing.Point(14, 28);
            this.tbFileName.Name = "tbFileName";
            this.tbFileName.Size = new System.Drawing.Size(267, 19);
            this.tbFileName.TabIndex = 12;
            // 
            // lbFileName
            // 
            this.lbFileName.AutoSize = true;
            this.lbFileName.Location = new System.Drawing.Point(15, 8);
            this.lbFileName.Name = "lbFileName";
            this.lbFileName.Size = new System.Drawing.Size(53, 12);
            this.lbFileName.TabIndex = 11;
            this.lbFileName.Text = "&Filename:";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miCapturePage,
            this.miUseDefLang,
            this.miDebugMode,
            this.toolStripMenuItem1,
            this.miExit});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(294, 98);
            // 
            // miCapturePage
            // 
            this.miCapturePage.Name = "miCapturePage";
            this.miCapturePage.Size = new System.Drawing.Size(293, 22);
            this.miCapturePage.Text = "Capture &Page";
            // 
            // miUseDefLang
            // 
            this.miUseDefLang.Name = "miUseDefLang";
            this.miUseDefLang.Size = new System.Drawing.Size(293, 22);
            this.miUseDefLang.Text = "Use Default Language (Experimental)";
            // 
            // miDebugMode
            // 
            this.miDebugMode.Name = "miDebugMode";
            this.miDebugMode.Size = new System.Drawing.Size(293, 22);
            this.miDebugMode.Text = "&Debug Mode";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(290, 6);
            // 
            // miExit
            // 
            this.miExit.Name = "miExit";
            this.miExit.ShortcutKeyDisplayString = "Ctrl+W";
            this.miExit.Size = new System.Drawing.Size(293, 22);
            this.miExit.Text = "E&xit";
            // 
            // lbPoint
            // 
            this.lbPoint.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lbPoint.AutoSize = true;
            this.lbPoint.Location = new System.Drawing.Point(207, 65);
            this.lbPoint.Name = "lbPoint";
            this.lbPoint.Size = new System.Drawing.Size(31, 12);
            this.lbPoint.TabIndex = 17;
            this.lbPoint.Text = "Point";
            // 
            // AutoTSOCRMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(297, 381);
            this.Controls.Add(this.tbInfo);
            this.Controls.Add(this.lbStatus);
            this.Controls.Add(this.tbText);
            this.Controls.Add(this.btnDoOCR);
            this.Controls.Add(this.tbFileName);
            this.Controls.Add(this.lbFileName);
            this.Controls.Add(this.lbPoint);
            this.Name = "AutoTSOCRMainForm";
            this.Text = "Auto TSOCR";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbInfo;
        private System.Windows.Forms.Label lbStatus;
        private System.Windows.Forms.TextBox tbText;
        private System.Windows.Forms.Button btnDoOCR;
        private System.Windows.Forms.TextBox tbFileName;
        private System.Windows.Forms.Label lbFileName;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem miCapturePage;
        private System.Windows.Forms.ToolStripMenuItem miUseDefLang;
        private System.Windows.Forms.ToolStripMenuItem miDebugMode;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem miExit;
        private System.Windows.Forms.Label lbPoint;

    }
}

