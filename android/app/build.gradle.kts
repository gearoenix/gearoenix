import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.gearoenix.app"
    compileSdk = 36
    ndkVersion = "29.0.14206865"
    defaultConfig {
        applicationId = "com.geareonix.engine_demo"
        minSdk = 21
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        ndk {
            abiFilters.clear()
            abiFilters += listOf("arm64-v8a")
        }

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_TOOLCHAIN=clang",
                    "-DANDROID_STL=c++_static",
                    "-DANDROID_PLATFORM=35",
                    "-DANDROID_ABI=arm64-v8a",
                    "-DANDROID_ARCH_ABI=arm64-v8a",
                    "-DANDROID_NDK=${android.ndkDirectory}",
                    "-DVCPKG_TARGET_TRIPLET=arm64-android",
                    "-DCMAKE_TOOLCHAIN_FILE=${System.getenv("VCPKG_ROOT")}/scripts/buildsystems/vcpkg.cmake",
                    "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${android.ndkDirectory}/build/cmake/android.toolchain.cmake",


//                    "-DGX_ENABLE_EXAMPLE_001_HELLO_TRIANGLE=True",
//                    "-DGX_ENABLE_EXAMPLE_002_HELLO_TEXTURE=True",
//                    "-DGX_ENABLE_EXAMPLE_003_HELLO_ECS=True",
                    "-DGX_ENABLE_EXAMPLE_004_HELLO_IMGUI=True",
//                    "-DGX_ENABLE_EXAMPLE_005_HELLO_GLTF=True",
//                    "-DGX_ENABLE_EXAMPLE_006_HELLO_SKYBOX=True",
//                    "-DGX_ENABLE_EXAMPLE_007_REFLECTION_PROBE=True",
//                    "-DGX_ENABLE_EXAMPLE_008_BAKED_REFLECTION_PROBE=True",
                )
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    kotlin {
        compilerOptions {
            jvmTarget = JvmTarget.JVM_21
        }
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

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
        }
    }
}

dependencies {
    implementation(fileTree("libs") { include("*.jar") })
}
