set(stblib_source_files

	src/include/SdkSTBFr.h
	src/include/SdkSTBPe.h
	src/include/SdkSTBTr.h
	src/include/STBCommonDef.h
	src/include/STBCommonType.h
	src/include/STB_Debug.h
	src/include/STBFaceInfo.h
	src/include/STBFrTypedef.h
	src/include/STBHandle.h
	src/include/STBMakeResult.h
	src/include/STBPeTypedef.h
	src/include/STBTracking.h
	src/include/STBTrTypedef.h
	src/include/STBTypedefInput.h
	src/include/STBTypedefOutput.h

	src/STB/Interface.c
	src/STB/Interface.h
	src/STB/STBAPI.c
	src/STB/STBAPI.h
	src/STB/STBFaceInfo.c
	src/STB/STBMakeResult.c
	src/STB/STBTracking.c
	src/STB/STBValidValue.c
	src/STB/STBValidValue.h

	src/STB_FaceRecognition/FrInterface.c
	src/STB_FaceRecognition/FrInterface.h
	src/STB_FaceRecognition/SdkSTBFr.c
	src/STB_FaceRecognition/SdkSTBFr.h
	src/STB_FaceRecognition/STBFrAPI.c
	src/STB_FaceRecognition/STBFrAPI.h
	src/STB_FaceRecognition/STBFrValidValue.c
	src/STB_FaceRecognition/STBFrValidValue.h

	src/STB_Property/PeInterface.c
	src/STB_Property/PeInterface.h
	src/STB_Property/SdkSTBPe.c
	src/STB_Property/SdkSTBPe.h
	src/STB_Property/STBPeAPI.c
	src/STB_Property/STBPeAPI.h
	src/STB_Property/STBPeValidValue.c
	src/STB_Property/STBPeValidValue.h

	src/STB_Tracker/SdkSTBTr.c
	src/STB_Tracker/SdkSTBTr.h
	src/STB_Tracker/STBTrAPI.c
	src/STB_Tracker/STBTrAPI.h
	src/STB_Tracker/STBTrValidValue.c
	src/STB_Tracker/STBTrValidValue.h
	src/STB_Tracker/TrInterface.c
	src/STB_Tracker/TrInterface.h

	usr_include/STBAPI.h
	usr_include/STBCommonDef.h
	usr_include/STBTypedef.h
)

include_directories("src/include/")
add_library(stblib ${stblib_source_files})
set(CMAKE_C_FLAGS  "-lm")
