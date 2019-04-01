function maxError maxError(interval)
maxError= 0;
for current = 1:length(interval)
tmp = lagrange('sin(x).*exp(x)',current-1,current);
maxError = [maxError tmp] 
end
maxError = max(maxError)
end