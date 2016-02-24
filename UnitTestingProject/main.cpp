#include <iostream>
#include <cstdlib>

#include <IL/il.h>
#include <IL/ilu.h>

#include "gtest/gtest.h"

#include "surface/ArtzyAlgorithm.h"
#include "digitalimage/BinaryVolume.h"
#include "digitalimage/Volume.h"

//TEST(DummyTest, JustCompilePlease) {
//	EXPECT_EQ(1, 1);
//}
//
//
//TEST(ArtzyAlgorithmTest, loadingVolume) {
//	using namespace surface;
//	using namespace digitalimage;
//	Volume *input_volume = new Volume(L"../Data/Cross/Cross", 5);
//	BinaryVolume *binary_volume = new BinaryVolume(*input_volume, 0.5);
//
//	EXPECT_EQ(input_volume->get_depth(), binary_volume->get_depth());
//	EXPECT_EQ(input_volume->get_width(), binary_volume->get_width());
//	EXPECT_EQ(input_volume->get_height(), binary_volume->get_height());
//
//	//Test number of voxels
//	EXPECT_EQ(binary_volume->get_width() * binary_volume->get_height() * binary_volume->get_depth(), 125) << "Number of voxels is: " << (binary_volume->get_width() * binary_volume->get_height() * binary_volume->get_depth()) << endl;
//	EXPECT_EQ(binary_volume->get_black_number(), 125 - 7) << "Number of black voxels is: " << binary_volume->get_black_number() << endl;
//	EXPECT_EQ(binary_volume->get_white_number(), 7) << "Number of white voxels is: " << binary_volume->get_white_number() << endl;
//	EXPECT_EQ(binary_volume->get_black_number() + binary_volume->get_white_number(), binary_volume->get_width() * binary_volume->get_height() * binary_volume->get_depth());
//
//	delete input_volume;
//	delete binary_volume;
//}
//
//TEST(ArtzyAlgorithmTest, artzyWorking1) {
//	using namespace surface;
//	using namespace digitalimage;
//	Volume *input_volume = new Volume(L"../Data/Cube/Cube", 3);
//	
//	EXPECT_EQ(input_volume->get_depth(), 3);
//	EXPECT_EQ(input_volume->get_width(), 3);
//	EXPECT_EQ(input_volume->get_height(), 3);
//
//	ArtzyAlgorithm segmentator(*input_volume, 0.5f);
//	BoundaryElement seed(1, 1, 1, TOP);
//
//	Mesh boundary = segmentator.extract_surface(seed);
//
//	EXPECT_EQ(boundary.get_num_vertex(), 8) << "Number of vertex is: " << boundary.get_num_vertex() << endl;
//
//	//delete input_volume;
//}
//
//TEST(ArtzyAlgorithmTest, artzyWorking2) {
//	using namespace surface;
//	using namespace digitalimage;
//	
//	Volume *input_volume = new Volume(L"../Data/Cross/Cross", 5);
//
//	EXPECT_EQ(input_volume->get_depth(), 5);
//	EXPECT_EQ(input_volume->get_width(), 5);
//	EXPECT_EQ(input_volume->get_height(), 5);
//
//	ArtzyAlgorithm segmentator(*input_volume, 0.5f);
//	BoundaryElement seed(2, 3, 2, TOP);
//
//	Mesh boundary = segmentator.extract_surface(seed);
//
//	EXPECT_EQ(boundary.get_num_vertex(), 32) << "Number of vertex is: " << boundary.get_num_vertex() << endl;
//
//	//Test writing to obj
//	boundary.write_to_obj(L"cross.obj", true);
//
//	//delete input_volume;
//}
//
//TEST(ArtzyAlgorithmTest, artzyWorking3) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	Volume input_volume(L"../Data/NonSquareSmall/NonSquareSmall", 3);
//
//	EXPECT_EQ(input_volume.get_depth(), 3);
//	EXPECT_EQ(input_volume.get_width(), 5);
//	EXPECT_EQ(input_volume.get_height(), 4);
//
//	ArtzyAlgorithm segmentator(input_volume, 0.5f);
//	
//	Mesh boundary1 = segmentator.extract_surface();
//	boundary1.write_to_obj(L"NonSquare01.obj", true);
//
//	EXPECT_EQ(boundary1.get_num_vertex(), 32) << "Number of vertex is: " << boundary1.get_num_vertex() << endl;
//
//	Mesh boundary2 = segmentator.extract_surface();
//	boundary2.write_to_obj(L"NonSquare02.obj", true);
//
//	EXPECT_EQ(boundary2.get_num_vertex(), 8) << "Number of vertex is: " << boundary2.get_num_vertex() << endl;
//
//}

//TEST(ArtzyAlgorithmTest, artzyMultipleSurfaces) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	Volume input_volume(L"../Data/Cropped/SL51_LCO5_3bar028.rec.16bitCropped", 132);
//
//	EXPECT_EQ(input_volume.get_depth(), 132);
//	EXPECT_EQ(input_volume.get_width(), 132);
//	EXPECT_EQ(input_volume.get_height(), 132);
//
//	ArtzyAlgorithm segmentator(input_volume, 0.5f);
//
//	int count = 0;
//	std::wstring base_name(L"Original-");
//
//	while(segmentator.more_surfaces()) {
//		Mesh component = segmentator.extract_surface(true);
//		component.write_to_obj(base_name + std::to_wstring(count++) + L".obj", true);
//		std::wcout << L"Boundary " << count << L" found" << L"\n";
//	}
//
//}

//TEST(BinaryImageTest, erodeTest) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	//Volume input_volume(L"../Data/Cropped/SL51_LCO5_3bar028.rec.16bitCropped", 132);
//	//Volume input_volume(L"../Data/sl47_NMC5_1c5bar/sl47_NMC5_1c5bar", 155);
//	//Volume input_volume(L"../Data/SL51_LCO5_3bar/fullTH", 132);
//	//Volume input_volume(L"../Data/SL43_C5_1c5bar/SL43_C5_1c5bar_Data", 184);
//	
//	Volume input_volume(L"../Data/Otsu_SingleRegion/6_PM50_BG_removed_ThresholdedOtsu_SingleRegion", 362);
//	EXPECT_EQ(input_volume.get_depth(), 362);
//	EXPECT_EQ(input_volume.get_width(), 511);
//	EXPECT_EQ(input_volume.get_height(), 454);
//	
//
//	//Extract binary volume
//	BinaryVolume input_binary_volume(input_volume, 0.5f);
//	std::wcout << L"Voxels: " << input_binary_volume.get_white_number() << L"\n";
//
//	//Erode an segment
//	int number_or_erotions = 0;
//	int previous_count = 0;
//	while (true) {
//		int component_counter = 0;
//		ArtzyAlgorithm segmentator(input_binary_volume);
//		while(segmentator.more_surfaces()) {
//			Mesh component = segmentator.extract_surface(false, false);
//			component_counter++;
//		}
//		std::wcout << L"Erosions: " << number_or_erotions << L" components: " << component_counter << L"\n";
//
//		if (component_counter == previous_count) {
//			break;
//		} else {
//			previous_count = component_counter;
//		}
//
//		//Erode this volume
//		std::wcout << L"Eroding..." << L"\n";
//		input_binary_volume.erode();
//		std::wcout << L"Voxels: " << input_binary_volume.get_white_number() << L"\n";
//		number_or_erotions++;
//	}
//}


//TEST(BinaryImageTest, erodeTest3) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	//Volume input_volume(L"../Data/Cropped/SL51_LCO5_3bar028.rec.16bitCropped", 132);
//	Volume input_volume(L"../Data/sl47_NMC5_1c5bar/sl47_NMC5_1c5bar", 155); //Spherical 296
//	//Volume input_volume(L"../Data/SL51_LCO5_3bar/fullTH", 132);
//	//Volume input_volume(L"../Data/SL43_C5_1c5bar/SL43_C5_1c5bar_Data", 184);
//	//Volume input_volume(L"../Data/Otsu_SingleRegion/6_PM50_BG_removed_ThresholdedOtsu_SingleRegion", 362);
//	EXPECT_EQ(input_volume.get_depth(), 155);
//	EXPECT_EQ(input_volume.get_width(), 296);
//	EXPECT_EQ(input_volume.get_height(), 296);
//
//	//Extract binary volume
//	BinaryVolume input_binary_volume(input_volume, 0.5f);
//
//	//Erode three times
//	input_binary_volume.erode();
//	input_binary_volume.erode();
//	input_binary_volume.erode();
//	input_binary_volume.erode();
//	input_binary_volume.erode();
//
//	ArtzyAlgorithm segmentator(input_binary_volume);
//
//	int count = 0;
//	std::wstring base_name(L"results\\Component-");
//	while(segmentator.more_surfaces()) {
//		BinaryVolume component = segmentator.get_segmented_volume(false);
//
//		//component.dilate();
//		//component.dilate();
//		//component.dilate();
//		//component.dilate();
//		//component.dilate();
//				
//		component.save_to_file(base_name + std::to_wstring(count++) + L"-");
//
//		std::wcout << L"Boundary " << count << L" found" << L"\n";
//
//		ArtzyAlgorithm segmentator_2(component);
//		Mesh surface = segmentator_2.extract_surface(true, true);
//		surface.write_to_obj(base_name + std::to_wstring(count) + L".obj", true);
//		count++;
//	}
//
//}

TEST (QuadMeshTest, creation) {
	using namespace surface;
	using namespace digitalimage;

	//Volume input_volume(L"../Data/test_volume/test_volume", 10); 
	Volume input_volume(L"../Data/worldcup/worldcup", 128);

	EXPECT_EQ(input_volume.get_depth(), 128);
	EXPECT_EQ(input_volume.get_width(), 128);
	EXPECT_EQ(input_volume.get_height(), 128);

	//Extract binary volume
	BinaryVolume input_binary_volume(input_volume, 0.5f);

	ArtzyAlgorithm segmentator(input_binary_volume);

	Mesh surface = segmentator.extract_surface(true, true);
	surface.write_to_obj(L"worldcupVoxel.obj", true);

}

//TEST(BinaryImageTest, erodeTest2) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	//Volume input_volume(L"../Data/Cropped/SL51_LCO5_3bar028.rec.16bitCropped", 132);
//	Volume input_volume(L"../Data/sl47_NMC5_1c5bar/sl47_NMC5_1c5bar", 155);
//	//Volume input_volume(L"../Data/SL51_LCO5_3bar/fullTH", 132);
//	//Volume input_volume(L"../Data/SL43_C5_1c5bar/SL43_C5_1c5bar_Data", 184);
//	//Volume input_volume(L"../Data/Otsu_SingleRegion/6_PM50_BG_removed_ThresholdedOtsu_SingleRegion", 362);
//	EXPECT_EQ(input_volume.get_depth(), 155);
//	EXPECT_EQ(input_volume.get_width(), 296);
//	EXPECT_EQ(input_volume.get_height(), 296);
//
//	//Extract binary volume
//	BinaryVolume input_binary_volume(input_volume, 0.5f);
//	
//
//	//Erode times
//	int times = 1;
//	for (int i = 0; i < times; i++) {
//		input_binary_volume.erode();
//	}
//	
//	//Dilate times to compensate
//	for (int i = 0; i < times; i++) {
//		input_binary_volume.dilate();
//	}
//
//
//	ArtzyAlgorithm segmentator(input_binary_volume);
//		
//	int count = 0;
//	std::wstring base_name(L"Eroded-");
//	
//	while(segmentator.more_surfaces()) {
//		Mesh component = segmentator.extract_surface(true, false);
//		//component.write_to_obj(base_name + std::to_wstring(count++) + L".obj", true);
//		std::wcout << L"Boundary " << count << L" found" << L"\n";
//		count++;
//	}
//	
//}

//TEST(BinaryImageTest, separateTest) {
//	using namespace surface;
//	using namespace digitalimage;
//
//	Volume input_volume(L"../Data/Cropped/SL51_LCO5_3bar028.rec.16bitCropped", 132);
//	//Volume input_volume(L"../Data/sl47_NMC5_1c5bar/sl47_NMC5_1c5bar", 155);
//	//Volume input_volume(L"../Data/SL51_LCO5_3bar/fullTH", 132);
//	//Volume input_volume(L"../Data/SL43_C5_1c5bar/SL43_C5_1c5bar_Data", 184);
//
//	//Volume input_volume(L"../Data/Otsu_SingleRegion/6_PM50_BG_removed_ThresholdedOtsu_SingleRegion", 362);
//	EXPECT_EQ(input_volume.get_depth(), 132);
//	EXPECT_EQ(input_volume.get_width(), 132);
//	EXPECT_EQ(input_volume.get_height(), 132);
//
//	//Extract binary volume
//	BinaryVolume input_binary_volume(input_volume, 0.5f);
//	std::wcout << L"Voxels: " << input_binary_volume.get_white_number() << L"\n";
//
//	BinaryVolume newVolume(input_volume.get_width(), input_volume.get_height(), input_volume.get_depth());
//	
//	ArtzyAlgorithm segmentator(input_binary_volume);
//	
//	Mesh component = segmentator.extract_surface(true, false);
//	newVolume = segmentator.get_segmented_volume();
//	newVolume.save_to_file(L"componentOriginal-");
//			
//}


int main(int argc, char* argv[]) { 
    testing::InitGoogleTest(&argc, argv);
	ilInit();
    iluInit();
	RUN_ALL_TESTS();
	std::getchar(); // keep console window open until Return keystroke
    return EXIT_SUCCESS;
}
