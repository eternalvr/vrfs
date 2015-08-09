namespace VRFSConnect
{
    partial class Form1
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.txtCOM2 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            this.cbPortsRMP = new System.Windows.Forms.ComboBox();
            this.txtSerialOut = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.cbPortsMCDU = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cbPortsECAM = new System.Windows.Forms.ComboBox();
            this.FSUIPCTimer = new System.Windows.Forms.Timer(this.components);
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusFSUIPC = new System.Windows.Forms.ToolStripStatusLabel();
            this.RMPStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.MCDUStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.ECAMStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.btnReload = new System.Windows.Forms.Button();
            this.listCom = new System.Windows.Forms.ListBox();
            this.TcasStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.cbPortsTcas = new System.Windows.Forms.ComboBox();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(39, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "label1";
            // 
            // timer1
            // 
            this.timer1.Interval = 250;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // txtCOM2
            // 
            this.txtCOM2.AutoSize = true;
            this.txtCOM2.Location = new System.Drawing.Point(678, 38);
            this.txtCOM2.Name = "txtCOM2";
            this.txtCOM2.Size = new System.Drawing.Size(35, 13);
            this.txtCOM2.TabIndex = 2;
            this.txtCOM2.Text = "label2";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(12, 54);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(244, 44);
            this.textBox1.TabIndex = 3;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(60, 228);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(121, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "COM Connect";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // cbPortsRMP
            // 
            this.cbPortsRMP.FormattingEnabled = true;
            this.cbPortsRMP.Location = new System.Drawing.Point(135, 119);
            this.cbPortsRMP.Name = "cbPortsRMP";
            this.cbPortsRMP.Size = new System.Drawing.Size(121, 21);
            this.cbPortsRMP.TabIndex = 5;
            // 
            // txtSerialOut
            // 
            this.txtSerialOut.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtSerialOut.Location = new System.Drawing.Point(681, 54);
            this.txtSerialOut.Multiline = true;
            this.txtSerialOut.Name = "txtSerialOut";
            this.txtSerialOut.Size = new System.Drawing.Size(286, 219);
            this.txtSerialOut.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 122);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "RMP";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 149);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(39, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "MCDU";
            // 
            // cbPortsMCDU
            // 
            this.cbPortsMCDU.FormattingEnabled = true;
            this.cbPortsMCDU.Location = new System.Drawing.Point(135, 146);
            this.cbPortsMCDU.Name = "cbPortsMCDU";
            this.cbPortsMCDU.Size = new System.Drawing.Size(121, 21);
            this.cbPortsMCDU.TabIndex = 8;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 176);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "ECAM";
            // 
            // cbPortsECAM
            // 
            this.cbPortsECAM.FormattingEnabled = true;
            this.cbPortsECAM.Location = new System.Drawing.Point(135, 173);
            this.cbPortsECAM.Name = "cbPortsECAM";
            this.cbPortsECAM.Size = new System.Drawing.Size(121, 21);
            this.cbPortsECAM.TabIndex = 10;
            // 
            // FSUIPCTimer
            // 
            this.FSUIPCTimer.Enabled = true;
            this.FSUIPCTimer.Interval = 1000;
            this.FSUIPCTimer.Tick += new System.EventHandler(this.FSUIPCTimer_Tick);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusFSUIPC,
            this.RMPStatusLabel,
            this.MCDUStatusLabel,
            this.ECAMStatusLabel,
            this.TcasStatusLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 282);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(992, 22);
            this.statusStrip1.TabIndex = 12;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusFSUIPC
            // 
            this.statusFSUIPC.BackColor = System.Drawing.Color.Yellow;
            this.statusFSUIPC.Name = "statusFSUIPC";
            this.statusFSUIPC.Size = new System.Drawing.Size(129, 17);
            this.statusFSUIPC.Text = "FSUIPC Not Connected";
            // 
            // RMPStatusLabel
            // 
            this.RMPStatusLabel.BackColor = System.Drawing.Color.Yellow;
            this.RMPStatusLabel.Name = "RMPStatusLabel";
            this.RMPStatusLabel.Size = new System.Drawing.Size(116, 17);
            this.RMPStatusLabel.Text = "RMP Not Connected";
            // 
            // MCDUStatusLabel
            // 
            this.MCDUStatusLabel.BackColor = System.Drawing.Color.Yellow;
            this.MCDUStatusLabel.Name = "MCDUStatusLabel";
            this.MCDUStatusLabel.Size = new System.Drawing.Size(126, 17);
            this.MCDUStatusLabel.Text = "MCDU Not Connected";
            // 
            // ECAMStatusLabel
            // 
            this.ECAMStatusLabel.BackColor = System.Drawing.Color.Yellow;
            this.ECAMStatusLabel.Name = "ECAMStatusLabel";
            this.ECAMStatusLabel.Size = new System.Drawing.Size(124, 17);
            this.ECAMStatusLabel.Text = "ECAM Not Connected";
            // 
            // btnReload
            // 
            this.btnReload.Location = new System.Drawing.Point(263, 116);
            this.btnReload.Name = "btnReload";
            this.btnReload.Size = new System.Drawing.Size(27, 78);
            this.btnReload.TabIndex = 13;
            this.btnReload.UseVisualStyleBackColor = true;
            this.btnReload.Click += new System.EventHandler(this.btnReload_Click);
            // 
            // listCom
            // 
            this.listCom.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listCom.FormattingEnabled = true;
            this.listCom.Location = new System.Drawing.Point(368, 94);
            this.listCom.Name = "listCom";
            this.listCom.Size = new System.Drawing.Size(120, 95);
            this.listCom.TabIndex = 14;
            this.listCom.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.listCom_DrawItem);
            // 
            // TcasStatusLabel
            // 
            this.TcasStatusLabel.BackColor = System.Drawing.Color.Yellow;
            this.TcasStatusLabel.Name = "TcasStatusLabel";
            this.TcasStatusLabel.Size = new System.Drawing.Size(120, 17);
            this.TcasStatusLabel.Text = "TCAS Not Connected";
            // 
            // cbPortsTcas
            // 
            this.cbPortsTcas.FormattingEnabled = true;
            this.cbPortsTcas.Location = new System.Drawing.Point(135, 201);
            this.cbPortsTcas.Name = "cbPortsTcas";
            this.cbPortsTcas.Size = new System.Drawing.Size(121, 21);
            this.cbPortsTcas.TabIndex = 15;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(992, 304);
            this.Controls.Add(this.cbPortsTcas);
            this.Controls.Add(this.listCom);
            this.Controls.Add(this.btnReload);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cbPortsECAM);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cbPortsMCDU);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtSerialOut);
            this.Controls.Add(this.cbPortsRMP);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.txtCOM2);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "VRFSConnect";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label txtCOM2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.ComboBox cbPortsRMP;
        private System.Windows.Forms.TextBox txtSerialOut;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cbPortsMCDU;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cbPortsECAM;
        private System.Windows.Forms.Timer FSUIPCTimer;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel statusFSUIPC;
        private System.Windows.Forms.ToolStripStatusLabel RMPStatusLabel;
        private System.Windows.Forms.ToolStripStatusLabel MCDUStatusLabel;
        private System.Windows.Forms.ToolStripStatusLabel ECAMStatusLabel;
        private System.Windows.Forms.Button btnReload;
        private System.Windows.Forms.ListBox listCom;
        private System.Windows.Forms.ToolStripStatusLabel TcasStatusLabel;
        private System.Windows.Forms.ComboBox cbPortsTcas;
    }
}

