namespace net.sf.loconetovertcp.lbsrvcfg
{
    partial class LbServerConfigForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LbServerConfigForm));
            this.comboBoxComPort = new System.Windows.Forms.ComboBox();
            this.buttonUpdatePorts = new System.Windows.Forms.Button();
            this.labelComPort = new System.Windows.Forms.Label();
            this.labelTcpPort = new System.Windows.Forms.Label();
            this.textBoxTcpPort = new System.Windows.Forms.TextBox();
            this.labelComSpeed = new System.Windows.Forms.Label();
            this.comboBoxComSpeed = new System.Windows.Forms.ComboBox();
            this.labelFlowCtrl = new System.Windows.Forms.Label();
            this.comboBoxFlowControl = new System.Windows.Forms.ComboBox();
            this.buttonSave = new System.Windows.Forms.Button();
            this.buttonDiscard = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // comboBoxComPort
            // 
            this.comboBoxComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxComPort.FormattingEnabled = true;
            this.comboBoxComPort.Location = new System.Drawing.Point(39, 30);
            this.comboBoxComPort.Name = "comboBoxComPort";
            this.comboBoxComPort.Size = new System.Drawing.Size(128, 21);
            this.comboBoxComPort.TabIndex = 1;
            // 
            // buttonUpdatePorts
            // 
            this.buttonUpdatePorts.Location = new System.Drawing.Point(188, 30);
            this.buttonUpdatePorts.Name = "buttonUpdatePorts";
            this.buttonUpdatePorts.Size = new System.Drawing.Size(52, 21);
            this.buttonUpdatePorts.TabIndex = 2;
            this.buttonUpdatePorts.Text = "&Update";
            this.buttonUpdatePorts.UseVisualStyleBackColor = true;
            this.buttonUpdatePorts.Click += new System.EventHandler(this.buttonUpdatePorts_Click);
            // 
            // labelComPort
            // 
            this.labelComPort.AutoSize = true;
            this.labelComPort.Location = new System.Drawing.Point(39, 10);
            this.labelComPort.Name = "labelComPort";
            this.labelComPort.Size = new System.Drawing.Size(70, 13);
            this.labelComPort.TabIndex = 0;
            this.labelComPort.Text = "COM_PORT:";
            // 
            // labelTcpPort
            // 
            this.labelTcpPort.AutoSize = true;
            this.labelTcpPort.Location = new System.Drawing.Point(39, 70);
            this.labelTcpPort.Name = "labelTcpPort";
            this.labelTcpPort.Size = new System.Drawing.Size(67, 13);
            this.labelTcpPort.TabIndex = 3;
            this.labelTcpPort.Text = "TCP_PORT:";
            // 
            // textBoxTcpPort
            // 
            this.textBoxTcpPort.Location = new System.Drawing.Point(39, 90);
            this.textBoxTcpPort.MaxLength = 8;
            this.textBoxTcpPort.Name = "textBoxTcpPort";
            this.textBoxTcpPort.Size = new System.Drawing.Size(128, 20);
            this.textBoxTcpPort.TabIndex = 4;
            this.textBoxTcpPort.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBoxTcpPort_KeyPress);
            // 
            // labelComSpeed
            // 
            this.labelComSpeed.AutoSize = true;
            this.labelComSpeed.Location = new System.Drawing.Point(39, 130);
            this.labelComSpeed.Name = "labelComSpeed";
            this.labelComSpeed.Size = new System.Drawing.Size(76, 13);
            this.labelComSpeed.TabIndex = 5;
            this.labelComSpeed.Text = "COM_SPEED:";
            // 
            // comboBoxComSpeed
            // 
            this.comboBoxComSpeed.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxComSpeed.FormattingEnabled = true;
            this.comboBoxComSpeed.Location = new System.Drawing.Point(39, 150);
            this.comboBoxComSpeed.Name = "comboBoxComSpeed";
            this.comboBoxComSpeed.Size = new System.Drawing.Size(128, 21);
            this.comboBoxComSpeed.TabIndex = 6;
            // 
            // labelFlowCtrl
            // 
            this.labelFlowCtrl.AutoSize = true;
            this.labelFlowCtrl.Location = new System.Drawing.Point(39, 190);
            this.labelFlowCtrl.Name = "labelFlowCtrl";
            this.labelFlowCtrl.Size = new System.Drawing.Size(75, 13);
            this.labelFlowCtrl.TabIndex = 7;
            this.labelFlowCtrl.Text = "FLOW_CNTL:";
            // 
            // comboBoxFlowControl
            // 
            this.comboBoxFlowControl.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxFlowControl.FormattingEnabled = true;
            this.comboBoxFlowControl.Location = new System.Drawing.Point(39, 210);
            this.comboBoxFlowControl.Name = "comboBoxFlowControl";
            this.comboBoxFlowControl.Size = new System.Drawing.Size(128, 21);
            this.comboBoxFlowControl.TabIndex = 8;
            // 
            // buttonSave
            // 
            this.buttonSave.Location = new System.Drawing.Point(27, 259);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(102, 23);
            this.buttonSave.TabIndex = 9;
            this.buttonSave.Text = "&Save";
            this.buttonSave.UseVisualStyleBackColor = true;
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // buttonDiscard
            // 
            this.buttonDiscard.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonDiscard.Location = new System.Drawing.Point(164, 259);
            this.buttonDiscard.Name = "buttonDiscard";
            this.buttonDiscard.Size = new System.Drawing.Size(102, 23);
            this.buttonDiscard.TabIndex = 10;
            this.buttonDiscard.Text = "&Discard";
            this.buttonDiscard.UseVisualStyleBackColor = true;
            this.buttonDiscard.Click += new System.EventHandler(this.buttonDiscard_Click);
            // 
            // LbServerConfigForm
            // 
            this.AcceptButton = this.buttonSave;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonDiscard;
            this.ClientSize = new System.Drawing.Size(292, 303);
            this.Controls.Add(this.buttonDiscard);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.comboBoxFlowControl);
            this.Controls.Add(this.labelFlowCtrl);
            this.Controls.Add(this.comboBoxComSpeed);
            this.Controls.Add(this.labelComSpeed);
            this.Controls.Add(this.textBoxTcpPort);
            this.Controls.Add(this.labelTcpPort);
            this.Controls.Add(this.labelComPort);
            this.Controls.Add(this.buttonUpdatePorts);
            this.Controls.Add(this.comboBoxComPort);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LbServerConfigForm";
            this.Text = "LbServer - Config";
            this.Load += new System.EventHandler(this.LbServerConfigForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBoxComPort;
        private System.Windows.Forms.Button buttonUpdatePorts;
        private System.Windows.Forms.Label labelComPort;
        private System.Windows.Forms.Label labelTcpPort;
        private System.Windows.Forms.TextBox textBoxTcpPort;
        private System.Windows.Forms.Label labelComSpeed;
        private System.Windows.Forms.ComboBox comboBoxComSpeed;
        private System.Windows.Forms.Label labelFlowCtrl;
        private System.Windows.Forms.ComboBox comboBoxFlowControl;
        private System.Windows.Forms.Button buttonSave;
        private System.Windows.Forms.Button buttonDiscard;
    }
}

