using System;
using System.Windows;
using System.Windows.Threading;
using System.Drawing;
using System.Drawing.Imaging;

using OpenTK;
using OpenTK.Graphics.OpenGL;

/*
 * Рисование модели методом плоского затенения
 */

namespace Lab4_2
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool _canDraw = false;
        private int _numOfVertices;
        private ObjectLoader _obj;
        private float _angle = 0.0f;
        private int _program;

        private int _uMvpMatrix;
        private int _uModelMatrix;
        private Matrix4 _mvpMatrix;
        private Matrix4 _modelMatrix;
        private Matrix4 _viewMatrix;
        private Matrix4 _projMatrix;

        private DispatcherTimer _dispatcherTimer;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void WindowsFormsHost_Initialized(object sender, EventArgs e)
        {
            glControl.MakeCurrent();
        }

        private void GLControl_Load(object sender, EventArgs e)
        {
            GL.ClearColor(0.2f, 0.3f, 0.2f, 1.0f);
            GL.Enable(EnableCap.DepthTest);

            if (!ShaderHelpers.InitShaders(
                "Shaders/vert.shader.glsl",
                "Shaders/frag.shader.glsl", out _program))
            {
                return;
            }

            if (!InitVertexBuffers(_program))
            {
                return;
            }

            InitTextures();

            _uMvpMatrix = GL.GetUniformLocation(_program, "uMvpMatrix");
            if (_uMvpMatrix < 0)
            {
                MessageBox.Show("Failed to get uMvpMatrix variable.");
                return;
            }

            _uModelMatrix = GL.GetUniformLocation(_program, "uModelMatrix");
            if (_uModelMatrix < 0)
            {
                MessageBox.Show("Failed to get uModelMatrix variable.");
                return;
            }

            _viewMatrix = Matrix4.LookAt(
                new Vector3(0.0f, 3.0f, 10.0f),
                new Vector3(0.0f, 0.0f, 0.0f),
                new Vector3(0.0f, 1.0f, 0.0f));

            _dispatcherTimer = new DispatcherTimer();
            _dispatcherTimer.Tick += OnUpdate;
            _dispatcherTimer.Interval = TimeSpan.FromMilliseconds(16.0);
            _dispatcherTimer.Start();

            _canDraw = true;
        }

        private void OnUpdate(object sender, EventArgs e)
        {
            _angle += 2.0f;
            _modelMatrix =
                Matrix4.CreateScale(3.0f) *
                Matrix4.CreateRotationY(MathHelper.DegreesToRadians(_angle));
            _mvpMatrix = _modelMatrix * _viewMatrix * _projMatrix;
            GL.UniformMatrix4(_uMvpMatrix, false, ref _mvpMatrix);
            GL.UniformMatrix4(_uModelMatrix, false, ref _modelMatrix);
            glControl.Invalidate();
        }

        private void GLControl_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            GL.Viewport(0, 0, glControl.Width, glControl.Height);
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            if (_canDraw)
            {
                GL.DrawArrays(PrimitiveType.Triangles, 0, _numOfVertices);
            }

            glControl.SwapBuffers();
        }

        private void GLControl_Resize(object sender, EventArgs e)
        {
            SetProjMatrix();
            glControl.Invalidate();
        }

        private bool InitVertexBuffers(int program)
        {
            _obj = new ObjectLoader();
            _obj.LoadModel("Models/monkey/monkey.obj");

            _numOfVertices = _obj.vertIndex.Count;

            int vbo;
            GL.GenBuffers(1, out vbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.BufferData(BufferTarget.ArrayBuffer,
                _obj.model.Length * sizeof(float),
                _obj.model, BufferUsageHint.StaticDraw);

            int textureOffset = _obj.vertIndex.Count * 3 * sizeof(float);
            int normalOffset = textureOffset + _obj.texIndex.Count * 2 * sizeof(float);

            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 0, 0);
            GL.EnableVertexAttribArray(0);

            GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, 0, textureOffset);
            GL.EnableVertexAttribArray(1);

            GL.VertexAttribPointer(2, 3, VertexAttribPointerType.Float, false, 0, normalOffset);
            GL.EnableVertexAttribArray(2);

            return true;
        }

        private void InitTextures()
        {
            int texture;
            GL.GenTextures(1, out texture);
            GL.BindTexture(TextureTarget.Texture2D, texture);

            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)All.Repeat);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)All.Repeat);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)All.Linear);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)All.Linear);

            string imageFileName = "Models/monkey/DefTexture.png";

            Bitmap image;
            try
            {
                image = new Bitmap(imageFileName);
            }
            catch (Exception)
            {
                MessageBox.Show("Failed to load the texture: " + imageFileName);
                return;
            }
            Rectangle rect = new Rectangle(0, 0, image.Width, image.Height);
            BitmapData data = image.LockBits(rect, ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppRgb);

            GL.TexImage2D(TextureTarget.Texture2D, 0,
                PixelInternalFormat.Rgb, image.Width, image.Height,
                0, OpenTK.Graphics.OpenGL.PixelFormat.Rgba,
                PixelType.UnsignedByte, data.Scan0);

            image.UnlockBits(data);
        }

        private void SetProjMatrix()
        {
            _projMatrix = Matrix4.CreatePerspectiveFieldOfView(
                MathHelper.DegreesToRadians(45.0f),
                (float)glControl.Width / glControl.Height, 0.1f, 1000.0f);
        }
    }
}
