
export aroma = love if love

import rectangle, setColor, getColor from aroma.graphics
import insert from table

_print = aroma.graphics.print

class Piece
  rot: {2, 2} -- rotation origin

  new: (@board, @ox=0, @oy=0, @pts=nil) =>
    error "need shape" if not @shape
    if @pts == nil
      @pts = {}
      for y=1,#@shape
        for x=1,#@shape[y]
          if @shape[y][x] > 0
            insert @pts, {x, y}
      @calc_shadow!

  calc_shadow: =>
    return if @is_shadow

    @shadow = with self.__class @board, @ox, @oy, [pt for pt in *@pts]
      .is_shadow = true

    while @shadow\try_move 0, 1 do nil
    nil

  -- clockwise unless flipped
  rotate: (flip=false)=>
    rot_x, rot_y = unpack @rot
    @pts = for pt in *@pts
      x = pt[1] - rot_x
      y = pt[2] - rot_y

      x,y = if flip
        y, -x
      else
        -y, x

      {x + rot_x, y + rot_y}


  each_pt: =>
    coroutine.wrap ->
      for pt in *@pts
        x, y = pt[1] + @ox, pt[2] + @oy
        coroutine.yield x,y

  try_rotate: =>
    @rotate!
    if @collides!
      @rotate true
    else
      @calc_shadow!

  try_move: (dx, dy) =>
    @ox += dx
    @oy += dy
    if @collides!
      @ox -= dx
      @oy -= dy
      false
    else
      @calc_shadow!
      true

  collides: =>
    for x, y in @each_pt!
      return true if x < 1 or x > @board.width or y < 1 or y > @board.height
      return true if @board.grid[x][y]

    false

  draw: =>
    if @shadow
      setColor 64,64,64
      for x,y in @shadow\each_pt!
        @board\draw_cell x, y

    setColor @color
    for x, y in @each_pt!
      @board\draw_cell x, y

class Ell extends Piece
  rot: {1, 2}
  color: {250, 94, 20}
  shape: {
    {1,1}
    {1,0}
    {1,0}
  }


class Seven extends Piece
  color: {229, 59, 116}
  shape: {
    {1,1}
    {0,1}
    {0,1}
  }


class Tee extends Piece
  color: {196, 229, 59}
  shape: {
    {0,1,0}
    {1,1,1}
  }


class Ess extends Piece
  color: {59, 229, 122}
  shape: {
    {0,1,1}
    {1,1,0}
  }

class Zee extends Piece
  color: {229, 83, 59}
  shape: {
    {1,1,0}
    {0,1,1}
  }

class Oo extends Piece
  color: {229, 219, 59}
  shape: {
    {1,1}
    {1,1}
  }


class Ii extends Piece
  rot: {1, 2}
  color: {59, 107, 229}
  shape: {
    {1}
    {1}
    {1}
    {1}
  }

class Board
  cell_size: 14
  padding: 1

  ox: 40
  oy: 40

  color: {200, 200, 200}

  new: (@width, @height) =>
    @grid = {}
    for x=1,@width
      col = {}
      for y=1,@height
        col[y] = false
      @grid[x] = col
    
    @center!

    s = (@cell_size + @padding)
    @real_width = @width * s
    @real_height = @height * s

  center: =>
    -- center in window
    size = @cell_size + @padding
    mx, my = @width * size + 2, @height * size + 2

    @ox = (aroma.graphics.getWidth! - mx) / 2
    @oy = (aroma.graphics.getHeight! - my) / 2

  -- returns the number of rows cleared
  set_piece: (piece) =>
    to_check = {}
    for x,y in piece\each_pt!
      to_check[y] = true
      @set x,y

    print "** setting piece"
    to_check = [y for y in pairs to_check]
    table.sort to_check

    count = 0
    for y in *to_check
      count += 1 if @check_row y
    count

  -- check if a row is completed
  check_row: (y) =>
    print "checking", y
    for i=1,@width
      return false if not @grid[i][y]

    print "got row"

    for yy=y,1,-1
      for xx=1,@width
        if yy < 1
          @grid[xx][yy] = false
        else
          @grid[xx][yy] = @grid[xx][yy - 1]
    true

  set: (x,y) =>
    @grid[x][y] = true

  draw_cell: (x, y, color) =>
    setColor color if color
    size = @cell_size + @padding
    rectangle "fill",
      @ox + (x - 1)*size, @oy + (y - 1)*size,
      @cell_size, @cell_size

  draw: =>
    size = @cell_size + @padding

    setColor 255,255,255

    rectangle "line",
      @ox - 1, @oy - 1,
      @width * size + 2, @height * size + 2
    
    setColor @color

    for y=1,@height
      for x=1,@width
        val = @grid[x][y]
        if val then @draw_cell x, y

bind_state = (state) ->
  for ev in *{"update", "draw", "keypressed", "focus"}
    aroma[ev] = state[ev] and (...) -> state[ev] state, ...

key_repeater = (key, initial=0.2, sustain=0.05) ->
  time = 0
  sustaining = false
  (dt) ->
    if aroma.keyboard.isDown key
      if time == 0
        with true
          time += dt
      else
        time += dt
        dt = if sustaining then sustain else initial
        if time >= dt
          time -= dt
          sustaining = true
          true
        else
          false
    else
      time = 0
      sustaining = false
      false

Game = nil

class GameOver
  new: (@game) =>

  draw: =>
    @game\draw!
    setColor 0,0,0, 128
    rectangle "fill", 0, 0, aroma.graphics.getWidth!, aroma.graphics.getHeight!

    setColor 255,255,255
    _print "Game over! - Enter to play again", 10, 10

  keypressed: (key) =>
    os.exit! if key == "escape" and love
    if key == "return"
      bind_state Game!

class Paused
  new: (@game) =>

  focus: (focus) =>
    bind_state @game if focus

  keypressed: (key) =>
    @focus true if key == "return"

  draw: =>
    setColor 255,255,255
    _print "Click to play", 10, 10
    @game\draw!

class Game
  pieces: {
    Ell, Seven, Tee, Ess, Zee, Oo, Ii
  }

  new: =>
    @speed = 0.4
    @board = Board 10, 22
    @score = 0

    @keys = {name, key_repeater(name) for name in *{
      "left", "right", "up", "down", " "
    }}

  -- called from key repeaters
  handle_key: (name) =>
    if @current_piece
      switch name
        when "left"
          @current_piece\try_move -1, 0
        when "right"
          @current_piece\try_move 1, 0
        when "down"
          @push_down!
        when "up"
          @current_piece\try_rotate!
        when " "
          while @push_down! do nil
          nil

  keypressed: (key, code) =>
    bind_state GameOver self if key == "q"
    os.exit! if key == "escape"

  push_down: =>
    return false if not @current_piece
    if not @current_piece\try_move 0, 1
      count = @board\set_piece @current_piece
      @speed -= 0.01 if count > 0
      @speed = math.max @speed, 0.05
      @score += count*count*50

      @current_piece = nil
      false
    else true

  update: (dt) =>
    if not @current_piece
      cls = @pieces[math.random 1, #@pieces]
      @current_piece = cls @board,
        cls.rot[1] + math.floor(@board.width / 3), 0

      -- game over if it immediately collides
      if @current_piece\collides!
        return bind_state GameOver self

      @last_tick = 0

    -- check for input
    for key, fn in pairs @keys
      @handle_key key if fn dt

    if @last_tick > @speed
      @last_tick -= @speed
      @push_down!

    @last_tick += dt

  draw: =>
    @board\draw!
    @current_piece\draw! if @current_piece
    setColor 200, 200, 200
    -- _print "rate: " .. @speed, @board.ox, @board.oy - 18
    _print "score: " .. @score, @board.ox, @board.oy + @board.real_height + 2

aroma.load = ->
  bind_state Paused Game!

