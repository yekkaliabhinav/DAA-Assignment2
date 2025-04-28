// src/components/CodeEditor.js
import React from 'react';
import Editor from '@monaco-editor/react';

const CodeEditor = ({ code }) => {
  const copyToClipboard = async () => {
    try {
      await navigator.clipboard.writeText(code);
      alert('Code copied to clipboard!');
    } catch (err) {
      console.error('Failed to copy code: ', err);
    }
  };

  return (
    <div style={{ position: 'relative', margin: '1rem 0' }}>
      <button
        onClick={copyToClipboard}
        style={{
          position: 'absolute',
          top: '0.5rem',
          right: '0.5rem',
          zIndex: 10,
          backgroundColor: '#444',
          color: '#fff',
          border: 'none',
          padding: '0.5rem 1rem',
          cursor: 'pointer',
          borderRadius: '4px'
        }}
      >
        Copy
      </button>

      <Editor
        height="500px"
        defaultLanguage="cpp"
        value={code}
        theme="vs-dark"
        options={{
          readOnly: true,
          minimap: { enabled: false }
        }}
      />
    </div>
  );
};

export default CodeEditor;
