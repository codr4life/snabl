def bench reps
  yield
  start = Time.now.to_f*1000
  reps.times { yield }
  (Time.now.to_f*1000 - start).to_i
end
