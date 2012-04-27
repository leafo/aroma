require "sitegen"
site = sitegen.create_site =>
  @title = "Hello World"
  add "reference.moon"

site\write!
