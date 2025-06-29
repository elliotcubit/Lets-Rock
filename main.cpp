
#include "buttplugclient.h"
#include "GG.h"

#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>

#pragma comment(lib, "crypt32")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")
#define P1_COMBO_OFFSET 0x6d4e86
#define P2_COMBO_OFFSET 0x6d4d3e

BYTE * base;
double lastIntensity = 0;

Client * c;
std::vector<DeviceClass> devices;
std::mutex devices_mutex;

void messageHandler(mhl::Messages msg) {
    // drop messages
}

void connectToIntiface() {
    (*c).connect(messageHandler);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (;;) {
        (*c).requestDeviceList();
        (*c).startScan();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        (*c).stopScan();
        devices_mutex.lock();
        devices = (*c).getDevices();
        devices_mutex.unlock();

        // Re-fetch device list every 10 seconds - sometimes the connection can be flaky
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void set(double intensity) {
    std::lock_guard<std::mutex> guard(devices_mutex);
    if (!devices.empty()) {
        (*c).sendScalar(devices[0], intensity);
    }
}

void hook(void) {
    unsigned char p1Counter = 0;
    unsigned char  p2Counter = 0;
    unsigned char  hitCount = 0;

    memcpy(&p1Counter, base+P1_COMBO_OFFSET, 1);
    memcpy(&p2Counter, base+P2_COMBO_OFFSET, 1);

    if (p1Counter < p2Counter) {
        hitCount = p2Counter;
    } else {
        hitCount = p1Counter;
    }

    double intensity = 0;
    intensity = ((double)hitCount) / 15.0;
    if (intensity > 1) {
        intensity = 1;
    }
    if (intensity <= 0) {
        intensity = 0;
    }

    // If the intensity changed, fire off a message
    if (intensity != lastIntensity) {
        lastIntensity = intensity;
        std::thread t2(set, intensity);
        t2.detach();
    }
}

void Patch(void) {
    c = new Client("ws://localhost", 12345);
    std::thread t1(connectToIntiface);
    t1.detach();
    AssignToGameLoop(hook);
    return;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        base = GetBase();
        Patch();
    }
    return TRUE;
}