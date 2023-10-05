#include <fstream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

struct V3
{
	float x, y, z;
};

int safe_stoi(const string& input)
{
	if (input.empty())
	{
		return INT_MAX;
	}
	else
	{
		return stoi(input);
	}
}

float safe_stof(const string& input)
{
	if (input.empty())
	{
		return FLT_MAX;
	}
	else
	{
		return stof(input);
	}
}

vector<string> split(const string& input, const string& delimiters, bool includeEmptyString = false)
{
	vector<string> result;
	string piece;
	for (auto c : input)
	{
		bool contains = false;
		for (auto d : delimiters)
		{
			if (d == c)
			{
				contains = true;
				break;
			}
		}

		if (contains == false)
		{
			piece += c;
		}
		else
		{
			if (includeEmptyString || piece.empty() == false)
			{
				result.push_back(piece);
				piece.clear();
			}
		}
	}
	if (piece.empty() == false)
	{
		result.push_back(piece);
	}

	return result;
}

void ReadBinarySTL(const string& fileName, vector<V3>& vertices, vector<vector<int>>& indices)
{
	FILE* fp = nullptr;
	fopen_s(&fp, fileName.c_str(), "rb");
	if (fp != nullptr)
	{
		char header[80];
		memset(header, 0, 80);
		fread_s(header, 80, 80, 1, fp);

		int nof = 0;
		fread_s(&nof, 4, 4, 1, fp);

		for (size_t i = 0; i < nof; i++)
		{
			V3 fn, v0, v1, v2;
			short dummy;

			fread_s(&fn, 12, 12, 1, fp);
			fread_s(&v0, 12, 12, 1, fp);
			fread_s(&v1, 12, 12, 1, fp);
			fread_s(&v2, 12, 12, 1, fp);
			fread_s(&dummy, 2, 2, 1, fp);

			int i0 = vertices.size();
			vertices.push_back(v0);
			int i1 = vertices.size();
			vertices.push_back(v1);
			int i2 = vertices.size();
			vertices.push_back(v2);

			indices.emplace_back(vector<int>{ i0, i1, i2 });
		}
	}
}

void ReadASCIISTL(const string& fileName, vector<V3>& vertices, vector<vector<int>>& indices)
{
	ifstream ifs(fileName);
	stringstream buffer;
	buffer << ifs.rdbuf();

	V3 fn;
	V3 verts[3];
	int vertex_index = 0;

	string line;
	while (buffer.good())
	{
		getline(buffer, line);
		if (line.empty())
			continue;

		auto words = split(line, " \t");
		if (words[0] == "facet")
		{
			if (words[1] == "normal")
			{
				fn.x = safe_stof(words[2]);
				fn.y = safe_stof(words[3]);
				fn.z = safe_stof(words[4]);
			}
		}
		else if (words[0] == "vertex")
		{
			verts[vertex_index].x = safe_stof(words[1]);
			verts[vertex_index].y = safe_stof(words[2]);
			vertices[vertex_index].z = safe_stof(words[3]);
			vertex_index++;
		}
		else if (words[0] == "endfacet")
		{
			int i0 = vertices.size();
			vertices.push_back(vertices[0]);
			int i1 = vertices.size();
			vertices.push_back(vertices[1]);
			int i2 = vertices.size();
			vertices.push_back(vertices[2]);

			indices.emplace_back(vector<int>{ i0, i1, i2 });

			vertex_index = 0;
		}
	}
}

void ReadSTL(const string& fileName, vector<V3>& vertices, vector<vector<int>>& indices)
{
	ifstream ifs(fileName);
	if (ifs.is_open() == false)
		return;

	string solid = "      ";
	ifs.read(&solid[0], 6);

	if (solid == "solid ")
	{
		ifs.close();
		ReadASCIISTL(fileName, vertices, indices);
	}
	else
	{
		ifs.close();
		ReadBinarySTL(fileName, vertices, indices);
	}
}

int main()
{
	polyscope::init();

	vector<V3> vertices;
	vector<vector<int>> indices;
	ReadSTL("C:\\Resources\\3D\\STL\\a.stl", vertices, indices);

	polyscope::registerSurfaceMesh("triangle", vertices, indices);

	polyscope::registerSurfaceMesh("line", vector<V3>{ {0.0f, 0.0f, 0.0f}, { 10.0f, 0.0f, 0.0f }, { 5.0f, 7.5f, 0.0f } }, vector<vector<int>>{ {0, 1, 2} });

	ImGuiIO& io = ImGui::GetIO();

	if (io.KeyCtrl && polyscope::render::engine->isKeyPressed('s')) {
		cout << "test" << endl;
	}

	polyscope::show();

	return 0;
}
