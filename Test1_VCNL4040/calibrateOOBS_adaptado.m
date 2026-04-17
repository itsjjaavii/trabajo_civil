clear
clc
close all

% Estandares de calibracion en NTU
standards = [0, 100, 250, 500, 1000]';

% Seleccionar archivo .TXT generado por firmware_calibracion.ino
[file, path] = uigetfile('/*.TXT', 'Selecciona el archivo de calibracion');
filepath = fullfile(path, file);

% Leer numero de serie del sensor
fid = fopen(filepath);
sn = 0;
for j = 1:5
    tline = fgetl(fid);
    if contains(tline, 'OpenOBS SN:')
        sn = str2double(tline(12:end));
        break
    end
end
fclose(fid);

% Leer datos ignorando lineas que empiezan con '#'
opts = detectImportOptions(filepath, 'CommentStyle', '#');
data = readtable(filepath, opts);

% Renombrar columnas por si acaso
% Esperadas: index, time, millis, backscatter, ambient_light, pressure, water_temp
disp('Columnas encontradas:')
disp(data.Properties.VariableNames)

% --- Autocalibracion usando presion ---
% La presion sube cuando sumerges el sensor en cada vaso
% Convertimos de unidades raw (x100 mbar) a mbar
data.pressure_mbar = double(data.pressure) / 100.0;
data.pressure_mbar(data.pressure_mbar > 2000) = NaN; % Eliminar valores irreales

% Detectar periodos sumergidos usando cambios de presion
pressureTrend    = movmedian(data.pressure_mbar, ...
                             max(3, floor(length(data.pressure_mbar)/10)), ...
                             'omitnan');
pressureDetrend  = data.pressure_mbar - pressureTrend;
pressureDetrend(pressureDetrend < 0) = 0;

% Umbral: 5 mbar ~ 5 cm de agua (ajusta si es necesario)
pressureThreshold = double(pressureDetrend > 5);
TF = find(ischange(pressureThreshold));

% Eliminar cambios muy cortos (ruido)
if numel(TF) > 1
    shortChanges = find(diff(TF) < 5);
    if any(shortChanges)
        TF(unique([shortChanges; shortChanges+1])) = [];
    end
end

autoCal = 0;
if numel(TF) == (numel(standards) * 2)
    autoCal = 1;
    autoCalText = sprintf(['Autocalibracion exitosa con datos de presion.\n' ...
        'Verifica que las regiones coloreadas correspondan a cada estandar.']);
else
    autoCalText = sprintf(['Autocalibracion fallida (encontrados %d cambios, esperados %d).\n' ...
        'Usa la herramienta brush para seleccionar cada estandar manualmente\n' ...
        'y exportalos como variables a, b, c, d, e.'], numel(TF), numel(standards)*2);
end

% --- Grafico ---
figure
set(gcf, 'Units', 'normalized', 'Position', [0.1 0.1 0.8 0.6])
hold on

measured = zeros(numel(standards), 2); % [media, std] por estandar

if autoCal
    colors = lines(numel(standards));
    for i = 1:numel(standards)
        idx = TF(i*2-1)+1 : TF(i*2)-1;
        measured(i,:) = [mean(data.backscatter(idx)), std(data.backscatter(idx))];
        plot(data.millis(idx)/1000, data.backscatter(idx), 'o', ...
            'Color', colors(i,:), 'HandleVisibility', 'off')
        text(mean(data.millis(idx)/1000), measured(i,1) + 500, ...
            sprintf('%d NTU', standards(i)), ...
            'FontSize', 12, 'HorizontalAlignment', 'center')
    end
end

plot(data.millis/1000, data.backscatter, 'b.', 'MarkerSize', 6)
text(0.05, 0.9, autoCalText, 'Units', 'normalized', 'FontSize', 12)
xlabel('Tiempo (s)')
ylabel('Backscatter (DN raw VCNL4040)')
title(sprintf('Calibracion - Sensor SN %03d', sn))
zoom on

%% --- Calcular y guardar calibracion ---
% Ejecuta esta seccion despues de verificar o corregir las regiones

% Si usaste brush, descomenta y reemplaza segun corresponda:
% if exist('a','var'), measured(1,:) = [mean(a{:,4}), std(a{:,4})]; end
% if exist('b','var'), measured(2,:) = [mean(b{:,4}), std(b{:,4})]; end
% if exist('c','var'), measured(3,:) = [mean(c{:,4}), std(c{:,4})]; end
% if exist('d','var'), measured(4,:) = [mean(d{:,4}), std(d{:,4})]; end
% if exist('e','var'), measured(5,:) = [mean(e{:,4}), std(e{:,4})]; end

if autoCal && all(measured(:,1) > 0)
    lm = fitlm(measured(:,1), standards);
    
    cal_slope     = lm.Coefficients.Estimate(2);
    cal_intercept = lm.Coefficients.Estimate(1);
    cal_R2        = lm.Rsquared.Ordinary;
    
    fprintf('\n=== RESULTADO DE CALIBRACION ===\n')
    fprintf('NTU = %.6f * DN + %.4f\n', cal_slope, cal_intercept)
    fprintf('R^2 = %.4f\n', cal_R2)
    fprintf('\nCopia estos valores al firmware:\n')
    fprintf('const float CAL_SLOPE     = %.6ff;\n', cal_slope)
    fprintf('const float CAL_INTERCEPT = %.4ff;\n', cal_intercept)
    
    % Grafico de calibracion
    figure
    hold on
    errorbar(measured(:,1), standards, measured(:,2)*3, 'ko', ...
        'LineWidth', 1.5, 'MarkerSize', 8)
    x_range = linspace(min(measured(:,1)), max(measured(:,1)), 100);
    plot(x_range, cal_slope*x_range + cal_intercept, 'r-', 'LineWidth', 2)
    xlabel('Backscatter medido (DN)')
    ylabel('Estandar (NTU)')
    title(sprintf('Calibracion SN %03d — R^2 = %.4f', sn, cal_R2))
    legend('Estandares (±3σ)', 'Ajuste lineal', 'Location', 'northwest')
    grid on
    box on
    
    % Guardar calibracion
    save_path = fullfile(path, sprintf('cal_SN%03d.mat', sn));
    save(save_path, 'measured', 'standards', 'lm', 'cal_slope', 'cal_intercept', 'cal_R2')
    fprintf('\nCalibracion guardada en: %s\n', save_path)
end