using System.IO;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Globalization;
using System.Windows.Forms;

namespace Lab4
{
    class ObjectLoader
    {
        // Массив для хранения модели в виде последовательности координат вершин, текстур и нормалей
        public float[] model;

        // Списки для хранения координат вершин и текстурных координат
        public List<float> vertCoord = new List<float>();
        public List<float> texCoord = new List<float>();

        // Списки для хранения индексов вершин, текстурных координат и нормалей
        public List<int> vertIndex = new List<int>();
        public List<int> texIndex = new List<int>();
        public List<int> normIndex = new List<int>();

        // Приватные списки для временного хранения данных
        private List<float> _model = new List<float>();
        private List<List<int>> _vertIndex = new List<List<int>>();
        private List<List<int>> _texIndex = new List<List<int>>();
        private List<List<int>> _normIndex = new List<List<int>>();
        private List<List<float>> _vertCoords = new List<List<float>>();
        private List<List<float>> _texCoords = new List<List<float>>();
        private List<List<float>> _normCoords = new List<List<float>>();

        private const string WHITESPACE_RE = @"\s+";

        // Метод для загрузки модели из файла
        public void LoadModel(string filePath)
        {
            if (!File.Exists(filePath))
            {
                MessageBox.Show("Failed to open the file: " + filePath);
                return;
            }

            using (var sr = new StreamReader(filePath))
            {
                string data = sr.ReadToEnd();
                string[] lines = data.Split(new char[] { '\n' });
                CultureInfo culture = new CultureInfo("en-US");

                // Парсинг данных из файла
                for (int i = 0; i < lines.Length; i++)
                {
                    if (lines[i].StartsWith("#")) continue;
                    string line = lines[i].Trim();
                    List<string> values = new List<string>(Regex.Split(line, WHITESPACE_RE));
                    if (values.Count == 0) continue;

                    if (values[0] == "v")
                    {
                        // Обработка вершин
                        float x = float.Parse(values[1], culture);
                        float y = float.Parse(values[2], culture);
                        float z = float.Parse(values[3], culture);
                        _vertCoords.Add(new List<float>() { x, y, z });
                    }
                    if (values[0] == "vt")
                    {
                        // Обработка текстурных координат
                        float u = float.Parse(values[1], culture);
                        float v = float.Parse(values[2], culture);
                        _texCoords.Add(new List<float>() { u, v });
                    }
                    if (values[0] == "vn")
                    {
                        // Обработка нормалей
                        float x = float.Parse(values[1], culture);
                        float y = float.Parse(values[2], culture);
                        float z = float.Parse(values[3], culture);
                        _normCoords.Add(new List<float>() { x, y, z });
                    }
                    if (values[0] == "f")
                    {
                        // Обработка индексов вершин, текстурных координат и нормалей для каждой грани
                        List<int> face_i = new List<int>();
                        List<int> tex_i = new List<int>();
                        List<int> norm_i = new List<int>();

                        for (int j = 1; j < 4; j++)
                        {
                            string[] w = values[j].Split(new char[] { '/' });
                            face_i.Add(int.Parse(w[0]) - 1);
                            tex_i.Add(int.Parse(w[1]) - 1);
                            norm_i.Add(int.Parse(w[2]) - 1);
                        }
                        _vertIndex.Add(face_i);
                        _texIndex.Add(tex_i);
                        _normIndex.Add(norm_i);
                    }
                }

                // Объединение данных в модель
                for (int i = 0; i < _vertIndex.Count; i++)
                {
                    for (int j = 0; j < _vertIndex[i].Count; j++)
                    {
                        vertIndex.Add(_vertIndex[i][j]);
                    }
                }

                for (int i = 0; i < _texIndex.Count; i++)
                {
                    for (int j = 0; j < _texIndex[i].Count; j++)
                    {
                        texIndex.Add(_texIndex[i][j]);
                    }
                }

                for (int i = 0; i < _normIndex.Count; i++)
                {
                    for (int j = 0; j < _normIndex[i].Count; j++)
                    {
                        normIndex.Add(_normIndex[i][j]);
                    }
                }

                for (int i = 0; i < vertIndex.Count; i++)
                {
                    int index = vertIndex[i];
                    List<float> coords = _vertCoords[index];
                    for (int j = 0; j < coords.Count; j++)
                    {
                        _model.Add(coords[j]);
                        vertCoord.Add(coords[j]);
                    }
                }

                for (int i = 0; i < texIndex.Count; i++)
                {
                    int index = texIndex[i];
                    List<float> coords = _texCoords[index];
                    for (int j = 0; j < coords.Count; j++)
                    {
                        _model.Add(coords[j]);
                        texCoord.Add(coords[j]);
                    }
                }

                for (int i = 0; i < normIndex.Count; i++)
                {
                    int index = normIndex[i];
                    List<float> coords = _normCoords[index];
                    for (int j = 0; j < coords.Count; j++)
                    {
                        _model.Add(coords[j]);
                    }
                }
                model = _model.ToArray(); // Преобразование модели в массив
            }
        }
    }
}
