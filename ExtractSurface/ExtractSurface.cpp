#include <iostream>
#include <cstdlib>
#include <string>
#include "surface/ArtzyAlgorithm.h"
#include "digitalimage/BinaryVolume.h"
#include "digitalimage/BoundaryElement.h"

using namespace std;
using namespace digitalimage;
using namespace surface;

struct Arguments {
	wstring input_volume;
	unsigned int volume_width;
	unsigned int volume_height;
	unsigned int volume_depth;
	float desired;
	BoundaryElement seed;
	wstring output_mesh;
};

void init();
const Arguments parse_arguments(const int& argc, char* argv[]);
void print_usage();

int main(int argc, char* argv[]) {
	//Init data structures and libraries
	init();

	//Parse arguments
	if (argc < 5) {
		print_usage();
		return EXIT_FAILURE;
	}
	Arguments args = parse_arguments(argc, argv);

	//Extract binary volume
	BinaryVolume input_binary_volume(args.input_volume, args.volume_width, args.volume_height, args.volume_depth);
	//Extract surface
	ArtzyAlgorithm segmentator(input_binary_volume);

	//Write the output
	Mesh surface = segmentator.extract_surface(true, true);
	surface.rescale_and_center(args.desired);
	surface.write_to_obj(args.output_mesh, true);
	wcout << L"Output written to: " << args.output_mesh << endl;
	
	return EXIT_SUCCESS;
}

void init() {
	//ilInit();
	//iluInit();
}

const Arguments parse_arguments(const int& argc, char* argv[]) {
	Arguments args;

	string* arguments = new string[argc];
	for (int i = 0; i < argc; ++i) {
		arguments[i] = (argv[i]);
	}

	wstring input_file_name = wstring(arguments[1].begin(), arguments[1].end());
	//wstring base_name = input_file_name.substr(0, input_file_name.length() - 4);
	wstring output_file_name = wstring(arguments[5].begin(), arguments[5].end());
	int width = std::stoi(arguments[2]);
	int height = std::stoi(arguments[3]);
	int depth = std::stoi(arguments[4]);
	float desired = 1.0f;
	
	if (argc >= 6) {
		desired = std::stof(arguments[6]);
	}
	

	//Calculate output name
	//wstring output_name = base_name + L".obj";

	//Fill the structure
	//TO DO: input seed
	args.input_volume = input_file_name;
	args.volume_width = width;
	args.volume_height = height;
	args.volume_depth = depth;
	args.output_mesh = output_file_name;
	args.desired = desired;

	return args;
}

void print_usage() {
	cout << "Usage: " << endl;
	cout << "ExtractSurface <base-name> <width> <height> <depth> [seed]" << endl;
}