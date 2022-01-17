// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef ARDUINOOCPP_H
#define ARDUINOOCPP_H

#include <ArduinoJson.h>
#include <memory>

#include <ArduinoOcpp/Core/ConfigurationOptions.h>
#include <ArduinoOcpp/Core/OcppTime.h>
#include <ArduinoOcpp/Core/OcppOperationCallbacks.h>
#include <ArduinoOcpp/Core/OcppOperationTimeout.h>
#include <ArduinoOcpp/Core/OcppSocket.h>

#include "Variants.h"

using ArduinoOcpp::OnReceiveConfListener;
using ArduinoOcpp::OnReceiveReqListener;
using ArduinoOcpp::OnSendConfListener;
using ArduinoOcpp::OnAbortListener;
using ArduinoOcpp::OnTimeoutListener;
using ArduinoOcpp::OnReceiveErrorListener;

using ArduinoOcpp::Timeout;

#if USE_FACADE

#ifndef AO_CUSTOM_WS
//uses links2004/WebSockets library
void OCPP_initialize(String CS_hostname, uint16_t CS_port, String CS_url, float V_eff = 230.f /*German grid*/, ArduinoOcpp::FilesystemOpt fsOpt = ArduinoOcpp::FilesystemOpt::Use_Mount_FormatOnFail, ArduinoOcpp::OcppClock system_time = ArduinoOcpp::Clocks::DEFAULT_CLOCK);
#endif

//Lets you use your own WebSocket implementation
void OCPP_initialize(ArduinoOcpp::OcppSocket& ocppSocket, float V_eff = 230.f /*German grid*/, ArduinoOcpp::FilesystemOpt fsOpt = ArduinoOcpp::FilesystemOpt::Use_Mount_FormatOnFail, ArduinoOcpp::OcppClock system_time = ArduinoOcpp::Clocks::DEFAULT_CLOCK);

//experimental; More testing required (help needed: it would be awesome if you can you publish your evaluation results on the GitHub page)
void OCPP_deinitialize();

void OCPP_loop();

/*
 * Provide hardware-related information to the library
 * 
 * The library needs a way to obtain information from your charger that changes over time. The
 * library calls following callbacks regularily (if they were set) to refresh its internal
 * status.
 * 
 * Set the callbacks once in your setup() function.
 */

void setPowerActiveImportSampler(std::function<float()> power);

void setEnergyActiveImportSampler(std::function<float()> energy);

void setEvRequestsEnergySampler(std::function<bool()> evRequestsEnergy);

void setConnectorEnergizedSampler(std::function<bool()> connectorEnergized);

void setConnectorPluggedSampler(std::function<bool()> connectorPlugged);

//void setConnectorFaultedSampler(std::function<bool()> connectorFailed);

void addConnectorErrorCodeSampler(std::function<const char *()> connectorErrorCode);

/*
 * React on calls by the library's internal functions
 * 
 * The library needs to set parameters on your charger on a regular basis or perform
 * functions triggered by the central system. The library uses the following callbacks
 * (if they were set) to perform updates or functions on your charger.
 * 
 * Set the callbacks once in your setup() function.
 */

void setOnChargingRateLimitChange(std::function<void(float)> chargingRateChanged);

void setOnUnlockConnector(std::function<bool()> unlockConnector); //true: success, false: failure

/*
 * React on CS-initiated operations
 * 
 * You can define custom behaviour in your integration which is executed every time the library
 * receives a CS-initiated operation. The following functions set a callback function for the
 * respective event. The library executes the callbacks always after its internal routines.
 * 
 * Set the callbacks once in your setup() function.
 */

void setOnSetChargingProfileRequest(OnReceiveReqListener onReceiveReq); //optional

void setOnRemoteStartTransactionSendConf(OnSendConfListener onSendConf); //important, energize the power plug here

void setOnRemoteStopTransactionSendConf(OnSendConfListener onSendConf); //important, de-energize the power plug here
void setOnRemoteStopTransactionReceiveReq(OnReceiveReqListener onReceiveReq); //optional, to de-energize the power plug immediately

void setOnResetSendConf(OnSendConfListener onSendConf); //important, reset your device here (i.e. call ESP.reset();)
void setOnResetReceiveReq(OnReceiveReqListener onReceiveReq); //alternative: start reset timer here

/*
 * Perform CP-initiated operations
 * 
 * Use following functions to send OCPP messages. Each function will adapt the library's
 * internal state appropriatley (e.g. after a successful StartTransaction request the library
 * will store the transactionId and send a StatusNotification).
 * 
 * On receipt of the .conf() response the library calls the callback function
 * "OnReceiveConfListener onConf" and passes the OCPP payload to it. The following functions
 * are non-blocking. Your application code will immediately resume with the subsequent code
 * in any case.
 */

void authorize(String &idTag, OnReceiveConfListener onConf = nullptr, OnAbortListener onAbort = nullptr, OnTimeoutListener onTimeout = nullptr, OnReceiveErrorListener onError = nullptr, std::unique_ptr<Timeout> timeout = nullptr);

void bootNotification(String chargePointModel, String chargePointVendor, OnReceiveConfListener onConf = nullptr, OnAbortListener onAbort = nullptr, OnTimeoutListener onTimeout = nullptr, OnReceiveErrorListener onError = nullptr, std::unique_ptr<Timeout> timeout = nullptr);

//The OCPP operation will include the given payload without modifying it. The library will delete the payload object by itself.
void bootNotification(DynamicJsonDocument *payload, OnReceiveConfListener onConf = nullptr, OnAbortListener onAbort = nullptr, OnTimeoutListener onTimeout = nullptr, OnReceiveErrorListener onError = nullptr, std::unique_ptr<Timeout> timeout = nullptr);

void startTransaction(OnReceiveConfListener onConf = nullptr, OnAbortListener onAbort = nullptr, OnTimeoutListener onTimeout = nullptr, OnReceiveErrorListener onError = nullptr, std::unique_ptr<Timeout> timeout = nullptr);

void stopTransaction(OnReceiveConfListener onConf = nullptr, OnAbortListener onAbort = nullptr, OnTimeoutListener onTimeout = nullptr, OnReceiveErrorListener onError = nullptr, std::unique_ptr<Timeout> timeout = nullptr);

/*
 * Provide hardware-related information II
 * 
 * When the EVSE state changes, you must notify the library in order to make StatusNotification
 * work properly.
 * 
 * Call these functions in your integration.
 */

//void startEvDrawsEnergy(); //<-- please use setEvRequestsEnergySampler(bool evRequestsEnergy());

//void stopEvDrawsEnergy(); //<-- please use setEvRequestsEnergySampler(bool evRequestsEnergy());

/*
 * Access information about the internal state of the library
 */

int getTransactionId(); //returns the ID of the current transaction. Returns -1 if called before or after an transaction

bool existsUnboundIdTag(); //returns if the user has given a valid Ocpp Charging Card which is not used for a transaction yet

bool isAvailable(); //if the charge point is operative or inoperative

#endif
#endif
