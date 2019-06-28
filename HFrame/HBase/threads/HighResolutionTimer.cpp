

HighResolutionTimer::HighResolutionTimer()                    { pimpl.reset (new Pimpl (*this)); }
HighResolutionTimer::~HighResolutionTimer()                   { stopTimer(); }

void HighResolutionTimer::startTimer (int periodMs)           { pimpl->start (jmax (1, periodMs)); }
void HighResolutionTimer::stopTimer()                         { pimpl->stop(); }

bool HighResolutionTimer::isTimerRunning() const noexcept     { return pimpl->periodMs != 0; }
int HighResolutionTimer::getTimerInterval() const noexcept    { return pimpl->periodMs; }


