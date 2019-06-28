function Initialize( ... )

    DelayY = tonumber(SKIN:GetVariable('DelayY'))
    H = tonumber(SKIN:GetVariable('H'))

    nBar = tonumber(SKIN:GetVariable('BarN'))-1
    delay = tonumber(SKIN:GetVariable('BarDelay'))
    speed = tonumber(SKIN:GetVariable('BarSpeed'))

    Bar = {}

    local z

    for i=0,nBar do

        if i < 10 then
            z='0'
        else
            z=''
        end

        Bar[i] = 
        {
            N = 0.001;
            T = 1;
            X = 1;
            Meter = 'OutFFT_L_Delay_'..z..i;
            Measure = 'MeasureAudioOut_L'..z..i
        }

        Bar[i+nBar+1] = 
        {
            N = 0.001;
            T = 1;
            X = 1;
            Meter = 'OutFFT_R_Delay_'..z..i;
            Measure = 'MeasureAudioOut_R'..z..i
        }

        SKIN:Bang('!SetOption', Bar[i].Meter, 'MeterStyle', 'StyleOutFFT_L_Delay')
        SKIN:Bang('!SetOption', Bar[i+nBar+1].Meter, 'MeterStyle', 'StyleOutFFT_R_Delay')
    end

    for i=0,nBar do
        

    end

    nBar = nBar*2+1

    f = true;
end


function Update()

    if f then
        for i=0,nBar do
            Bar[i].X = SKIN:GetMeter(Bar[i].Meter):GetX(true);
        end
        f = false
    end

    for i=0,nBar do

        N = SKIN:GetMeasure(Bar[i].Measure):GetValue()

        if Bar[i].N > N then
            Bar[i].T = Bar[i].T + 1;
            if Bar[i].T > delay then
                Bar[i].T = delay
                Bar[i].N = Bar[i].N - speed
            end
        else
            Bar[i].N = N;
            Bar[i].T = 0;
        end

        Y = (DelayY+H*(1-Bar[i].N))

        SKIN:Bang('!MoveMeter', Bar[i].X, Y, Bar[i].Meter)

    end

end

