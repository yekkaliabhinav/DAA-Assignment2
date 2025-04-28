// src/components/Navbar.js
import React from 'react';
import NavLinks from './NavLinks'; // or './NavLink' if you haven't renamed the file

const Navbar = () => {
  return (
    <nav style={{
      backgroundColor: '#282c34',
      width: '100%',
      padding: '0.75rem 2rem',
      display: 'flex',
      justifyContent: 'center'
    }}>
      <NavLinks />
    </nav>
  );
};

export default Navbar;
