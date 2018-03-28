function get_db_path(key)
  if key == "test" then
    return "/tmp/db.gdbm"
  end
end

function get_cfg(key)
  if key == "dbtype" then
	  return "2"
  end
  if key == "dbpath" then
	  return "/tmp/db.db"
  end
  if key == "logmask" then
	  return "0xFF"
  end
end
