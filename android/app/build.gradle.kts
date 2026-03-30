plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.gearoenix.app"
    compileSdk = 36
    ndkVersion = "30.0.14904198"
    defaultConfig {
        applicationId = "com.geareonix.engine_demo"
        minSdk = 21
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        ndk {
            abiFilters += "arm64-v8a"
        }

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_static",
                    "-DANDROID_PLATFORM=36",
                    "-DVCPKG_TARGET_TRIPLET=gearoenix-arm64-android",
                    "-DVCPKG_OVERLAY_TRIPLETS=${projectDir}/../../cmake/vcpkg/triplets",
                    "-DCMAKE_TOOLCHAIN_FILE=${System.getenv("VCPKG_ROOT")}/scripts/buildsystems/vcpkg.cmake",
                    "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${System.getenv("ANDROID_NDK_HOME")}/build/cmake/android.toolchain.cmake",

//                    "-DGX_ENABLE_EXAMPLE_001_HELLO_TRIANGLE=True",
//                    "-DGX_ENABLE_EXAMPLE_002_HELLO_TEXTURE=True",
//                    "-DGX_ENABLE_EXAMPLE_003_HELLO_ECS=True",
//                    "-DGX_ENABLE_EXAMPLE_004_HELLO_IMGUI=True",
//                    "-DGX_ENABLE_EXAMPLE_005_HELLO_GLTF=True",
//                    "-DGX_ENABLE_EXAMPLE_006_HELLO_SKYBOX=True",
//                    "-DGX_ENABLE_EXAMPLE_007_REFLECTION_PROBE=True",
                    "-DGX_ENABLE_EXAMPLE_008_BAKED_REFLECTION_PROBE=True",
                )
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    androidComponents {
        onVariants(selector().all()) { variant ->
            val cap = variant.name.replaceFirstChar { it.uppercase() }
            project.tasks.configureEach {
                if (name.contains("merge${cap}Assets")) {
                    dependsOn(project.tasks.named("externalNativeBuild${cap}"))
                }
            }
        }
    }

    buildTypes {
        release {
            isDebuggable = true
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    lint {
        abortOnError = false
    }

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "4.1.2"
        }
    }
}

dependencies {
    implementation(fileTree("libs") { include("*.jar") })
}
