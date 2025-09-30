#pragma once

void setupRoutes();

// handlers (используются при регистрации)
void handleRoot();
void handleLogPage();
void handleUart();
void handleUartClear();
void handleGetSetpoint();
void handleGetTemp();
void handleSetTemp();
void handleNotFound();
void handleGraphPage();
void handleGraphSvg();
void handleSensors();
void handlePids();

